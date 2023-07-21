#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <jsoncpp/json/json.h>
#include "../Common/util.hpp"
#include "../Common/log.hpp"
// #include "oj_model_file.hpp"
#include "oj_model_mysql.hpp"
#include "oj_view.hpp"
#include "../Common/httplib.h"

namespace ns_control
{
    using namespace ns_util;
    using namespace ns_log;
    using namespace ns_model;
    using namespace ns_view;
    using namespace httplib;

    // 提供编译服务的主机
    class Machine
    {
    public:
        std::string ip;  // 编译服务的ip
        int port;        // 编译服务的端口
        uint64_t load;   // 编译服务的负载数
        std::mutex *mtx; // mutex对象是禁止拷贝的，所以此处使用指针去拷贝
    public:
        Machine() : ip(""), port(0), load(0), mtx(nullptr)
        {
        }
        ~Machine() {}

    public:
        // 提升主机负载
        void IncLoad()
        {
            if (mtx) mtx->lock();
            load++;
            if (mtx) mtx->unlock();
        }
        // 降低主机负载
        void DecLoad()
        {
            if (mtx) mtx->lock();
            load--;
            if (mtx) mtx->unlock();
        }
        // 获取主机负载，意义不大只是为了统一接口
        uint64_t Load()
        {
            uint64_t _load = 0;
            if (mtx) mtx->lock();
            _load = load;
            if (mtx) mtx->unlock();
            return _load;
        }
        // 清空主机负载
        void ResetLoad()
        {
            if (mtx) mtx->lock();
            load = 0;
            if (mtx) mtx->unlock();
        }
    };
    const std::string service_machine = "./conf/service_machine.conf";
    // 负载均衡模块
    class LoadBlance
    {
    private:
        // 可以提供编译服务的所有主机均放在vector容器中
        // 使用下标对主机进行编号，下标充当当前主机的id
        std::vector<Machine> machines;
        // 所有在线的主机
        std::vector<int> online;
        // 所有离线的主机
        std::vector<int> offline;
        // 保护LoadBlance数据
        std::mutex mtx;

    public:
        LoadBlance()
        {
            assert(LoadConf(service_machine));
            LOG(INFO) << "加载" << service_machine << " 成功"
                      << "\n";
        }
        ~LoadBlance() {}

    public:
        bool LoadConf(const std::string &machine_conf)
        {
            std::ifstream in(machine_conf);
            if (!in.is_open())
            {
                LOG(FATAL) << " 加载 " << machine_conf << " 文件失败 "
                           << "\n";
                return false;
            }
            // 按行读取machine_conf中的ip和port，构建machine主机push进vector容器中的machines
            std::string line;
            while (std::getline(in, line))
            {
                std::vector<std::string> tokens;
                // 对每一行进行切分构建主机
                StringUtil::SplitString(line, &tokens, ":");
                if (tokens.size() != 2)
                {
                    LOG(WARNING) << " 切分 " << line << " 失败"
                                 << "\n";
                    continue;
                }
                Machine m;
                m.ip = tokens[0];
                m.port = atoi(tokens[1].c_str());
                m.load = 0;
                m.mtx = new std::mutex();

                online.push_back(machines.size());
                machines.push_back(m);
            }
            in.close();
            return true;
        }
        // id : 输出型参数
        // m : 输出型参数
        bool SmartChoice(int *id, Machine **m)
        {
            // 1. 使用选择好的主机(更新该主机的负载)
            // 2. 我们可能需要离线该主机
            mtx.lock();
            // 负载均衡的算法
            // 1. 随机数+hash
            // 2. 轮询+hash
            int online_num = online.size();
            if (online_num == 0)
            {
                mtx.unlock();
                LOG(FATAL) << " 所有后端主机的编译服务都已经挂掉了, 请运维同事检查一下"
                           << "\n";
                return false;
            }
            // 遍历找到负载最小的主机
            *id = online[0];
            *m = &machines[online[0]];
            uint64_t min_load = machines[online[0]].Load();
            for (int i = 1; i < online_num; i++)
            {
                uint64_t curr_load = machines[online[i]].Load();
                if (min_load > curr_load)
                {
                    min_load = curr_load;
                    *id = online[i];
                    *m = &machines[online[i]];
                }
            }
            mtx.unlock();
            return true;
        }
        void OnlineMachine()
        {
            // 上线所有主机
            mtx.lock();
            online.insert(online.end(), offline.begin(), offline.end());
            offline.erase(offline.begin(), offline.end());
            mtx.unlock();
            LOG(INFO) << "所有主机已上线" << "\n";
        }
        void OfflineMachine(int which)
        {
            // 在离线的同时有可能有用户正在进行SmartChoice，所以需要加锁保护
            mtx.lock();
            for (auto iter = online.begin(); iter != online.end(); iter++)
            {
                // 找到离线主机
                if (*iter == which)
                {
                    // 离线主机前需要将主机的负载的负载清空，否则在下一次上线主机时会使用之前已经存在的负载
                    // 应将主机负载清0，这样下一次上线时主机负载就是0了，选择主机时就会选择该主机
                    machines[which].ResetLoad();
                    online.erase(iter);
                    // offline.push_back(*iter); // 迭代器已经生效，不能再写*iter
                    offline.push_back(which);
                    // 因为有break的存在，一旦找到就会跳出了，所以暂时不考虑erase之后迭代器失效的问题
                    break;
                }
            }
            mtx.unlock();
        }
        // for test function
        void ShowMachines()
        {
            mtx.lock();
            std::cout << "当前在线主机列表: ";
            for (auto &id : online)
            {
                std::cout << id << " ";
            }
            std::cout << std::endl;

            std::cout << "当前离线主机列表: ";
            for (auto &id :offline)
            {
                std::cout << id << " ";
            }
            std::cout << std::endl;
            
            mtx.unlock();
        }
    };
    // 核心业务逻辑的控制器
    class Control
    {
    private:
        Model _model;            // 提供题目数据
        View _view;              // 将题目数据渲染成网页
        LoadBlance _load_blance; // 核心负载均衡器
    public:
        Control() {}
        ~Control() {}

    public:
        void RecoveryMachine()
        {
            _load_blance.OnlineMachine();
        }
        // 根据题目数据构建网页
        // html: 输出型参数
        bool AllQuestions(std::string *html)
        {
            bool ret = true;
            std::vector<struct Question> all;
            if (_model.GetAllQuestions(&all))
            {
                // 获取题目列表上来后对题目列表进行升序排序, 即对vector进行排序
                sort(all.begin(), all.end(), [](const struct Question& q1, const struct Question& q2){
                    // number是字符串类型
                    return atoi(q1.number.c_str()) < atoi(q2.number.c_str());
                });
                // 获取所有题目信息成功，将所有的题目构建成网页
                _view.AllExpandHtml(all, html);
            }
            else
            {
                *html = "获取题目列表失败";
                ret = false;
            }
            return ret;
        }
        bool Question(const std::string &number, std::string *html)
        {
            struct Question q;
            bool ret = true;
            if (_model.GetOneQuestion(number, &q))
            {
                // 获取指定题目信息成功，将所有的题目构建成网页
                _view.OneExpandHtml(q, html);
            }
            else
            {
                *html = "指定题目: " + number + "不存在!";
                ret = false;
            }
            return ret;
        }
        // id : 1
        // code : #include
        // input : ""
        void Judge(const std::string &number, const std::string in_json, std::string *out_json)
        {
            // 0. 根据题目编号，直接拿到对应的题目细节
            struct Question q;
            _model.GetOneQuestion(number, &q);

            // 1. 将in_json做反序列化工作，得到id、code、input
            Json::Reader reader;
            Json::Value in_value;
            reader.parse(in_json, in_value);
            std::string code = in_value["code"].asString();

            // 2. 重新拼装用户代码+测试用例代码，形成新的代码
            Json::Value compile_value;
            compile_value["input"] = in_value["input"].asString();
            // 将头部和尾部的代码做一个换行以却分开，否则可能会些编译问题
            compile_value["code"] = code + "\n" + q.tail;
            compile_value["cpu_limit"] = q.cpu_limit;
            compile_value["mem_limit"] = q.mem_limit;
            // 将形成的这个具有完整代码和时空要求的代码转成json串数据发送给compile_server服务
            Json::FastWriter writer;
            std::string compile_string = writer.write(compile_value);

            // 3. oj_control选择负载最小的主机(差错处理)
            // 规则: 一直选择直到有可用的主机，否则就是所有主机全部都挂掉了
            while (true)
            {
                int id = 0;
                Machine *m = nullptr;
                if (!_load_blance.SmartChoice(&id, &m))
                {
                    break;
                }
                // 4. 发起http请求，得到结果
                Client cli(m->ip, m->port);
                m->IncLoad();
                LOG(INFO) << " 选择主机成功, 主机id: " << id << " 详情: " << m->ip << ":" << m->port << " 当前主机负载: " << m->Load() << "\n";
                if (auto res = cli.Post("/compile_and_run", compile_string, "application/json;charset=utf-8"))
                {
                    // 有可能会出现请求失败
                    if (res->status == 200)
                    {
                        // 5. 将结果赋值给out_json
                        *out_json = res->body;
                        m->DecLoad();
                        LOG(INFO) << "请求编译和运行服务成功" << "\n";
                        break;
                    }
                }
                else
                {
                    LOG(ERROR) << "当前请求的主机id " << id << " 详情: " << m->ip << ":" << m->port << "可能已经挂掉了"
                               << "\n";
                    _load_blance.OfflineMachine(id);
                    _load_blance.ShowMachines(); // 仅仅用来测试
                }
            }
        }
    };
}
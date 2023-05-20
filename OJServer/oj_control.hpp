#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <fstream>
#include <cassert>
#include "../Common/util.hpp"
#include "../Common/log.hpp"
#include "oj_model.hpp"
#include "oj_view.hpp"

namespace ns_control
{
    using namespace ns_util;
    using namespace ns_log;
    using namespace ns_model;
    using namespace ns_view;

    // 提供编译服务的主机
    class Machine
    {
    public:
        std::string ip;  // 编译服务的ip
        int port;        // 编译服务的端口
        uint64_t load;   // 编译服务的负载数
        std::mutex *mtx; // mutex对象是禁止拷贝的，所以此处使用指针去拷贝
    public:
        Machine() : ip(""), port(0), load(0), mtc(nullptr)
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
            if (!in.open())
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
        bool SmartChioce(int *id, Machine **m)
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
            // 遍历找到最小负载的主机
            *id = online[0];
            *m = machines[online[0]];
            uint64_t min_load = machines[online[0]].Load();
            for (int i = 0; i < online_num; i++)
            {
                uint64_t curr_load = machines[online[i]].Load();
                if (min_load > curr_load)
                {
                    min_load = curr_load;
                    *id = online[i];
                    *m = machines[online[i]];   
                }
            }
            mtc.unlock();
            return true;
        }
        void OnlineMachine()
        {
        }
        void OfflineMachine()
        {
        }
    };
    // 核心业务逻辑的控制器
    class Control
    {
    private:
        Model _model;               // 提供题目数据
        View _view;                 // 将题目数据渲染成网页
        LoadBlance _load_blance;    // 核心负载均衡器
    public:
        Control() {}
        ~Control() {}

    public:
        // 根据题目数据构建网页
        // html: 输出型参数
        bool AllQuestions(std::string *html)
        {
            bool ret = true;
            std::vector<struct Question> all;
            if (_model.GetAllQuestions(&all))
            {
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
        void Judge(const std::string in_json, std::string *out_json)
        {
            // 1. 将in_json做反序列化工作，得到id、code、input
            // 2. 重新拼装用户代码+测试用例代码，形成新的代码
            // 3. oj_control选择负载最小的主机(差错处理)
            // 4. 发起http请求，得到结果
            // 5. 将结果赋值给out_json
        }
    };
}
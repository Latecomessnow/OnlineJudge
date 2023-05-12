#pragma once

#include <iostream>
#include <jsoncpp/json/json.h>
#include "compile.hpp"
#include "runner.hpp"
#include "../Common/log.hpp"
#include "../Common/util.hpp"

namespace ns_compile_and_run
{
    using namespace ns_compiler;
    using namespace ns_runner;
    using namespace ns_log;
    using namespace ns_util;
    class CompileAndRun
    {
    public:
        static std::string CodeToDesc(int code, const std::string &file_name)
        {
        }
        /***********************************************************
         * 输入:
         * code: 用户提交提交过来的代码
         * stdin: 用户有可能进行标准输入，可做扩展
         * cpu_limit: 时间要求
         * mem_limit: 空间要求
         *
         * 输出:
         * 必填:
         * status: 用户提交代码运行后的返回值
         * reason: 请求结果
         *
         * 选填:
         * stdout: 程序运行完的结果
         * stderr: 程序运行完的错误结果
         *
         * json可将结构化数据转为字符串，即进行网络中的序列化工作
         * 参数：
         * in_json: {"code": "#include...", "input": "","cpu_limit":1, "mem_limit":10240}
         * out_json: {"status":"0", "reason":"","stdout":"","stderr":"",}
         ***********************************************************/
        static void Start(const std::string &in_json, std::string *out_json)
        {
            // 进行反序列化工作
            Json::Value in_value;
            Json::Reader reader;
            reader.parse(in_json, in_value); // 最后再处理差错

            std::string code = in_value["code"].asString();
            std::string input = in_value["input"].asString();
            int cpu_limit = in_value["cpu_limit"].asInt();
            int mem_limit = in_value["mem_limit"].asInt();

            Json::Value out_value;

            // goto end之间不能定义变量
            int status_code = 0;
            int run_result = 0;

            std::string file_name; // 需要内部形成的唯一文件名

            if (code.size() == 0)
            {
                status_code = -1; // 代码为空
                goto END;
            }

            // 形成的文件名只具有唯一性，没有目录没有后缀
            // 毫秒级时间戳+原子性递增唯一值: 来保证唯一性
            file_name = FileUtil::UniqFileName();
            // 形成临时代码文件src
            if (!FileUtil::WritweFile(PathUtil::Src(file_name), code))
            {
                status_code = -2; // 未知错误
                goto END;
            }

            if (!Compiler::Compile(file_name))
            {
                // 编译失败
                status_code = -3; // 代码编译时发生错误
                goto END;
            }

            run_result = Runner::Run(file_name, cpu_limit, mem_limit);
            if (run_result > 0)
            {
                // 程序运行崩溃，收到某个信号了
                status_code = run_result;
            }
            else if (run_result < 0)
            {
                // 服务器内部代码发生错误
                status_code = -2; // 未知错误
            }
            else
            {
                // 运行成功
                status_code = 0;
            }
        END:
            out_value["status"] = status_code;
            out_value["reason"] = CodeToDesc(status_code, file_name);
            if (status_code == 0)
            {
                // 整个过程全部成功
                out_value["stdout"] = FileUtil::ReadFile(PathUtil::Stdout(file_name));
                out_value["stderr"] = FileUtil::ReadFile(PathUtil::Stderr(file_name));
            }
            // 反序列化工作
            Json::StyledWriter writer;
            *out_json = writer.write(out_value);

            /***********************************************
            if (code.size() == 0) // 代码文件出错
            {
                out_value["status"] = -1; // 代表用户提交代码为空
                out_value["reason"] = "用户提交代码为空";
                // 序列化过程
                return;
            }

            // 形成的文件名只具有唯一性，没有目录和后缀名
            // 毫秒级时间戳+原子性递增唯一值来保证唯一性
            std::string file_name = FileUtil::UniqFileName();

            // 形成临时的代码文件
            if (!FileUtil::WritweFile(PathUtil::Src(file_name), code))
            {
                out_value["status"] = -2; // 未知错误---不暴露给用户
                out_value["reason"] = "发生了未知错误";
                // 序列化过程
                return;
            }

            // 编译
            if (Compiler::Compile(file_name))
            {
                // 编译失败
                out_value["status"] = -3; // 代表代码编译时发生了错误
                // 将编译报错的的错误信息返回给用户(该信息已经存在了temp目录下的.compile_error文件)
                out_value["reason"] = FileUtil::ReadFile(PathUtil::CompileError(file_name));
                // 序列化过程
                return;
            }

            // 运行
            int code = Runner::Run(file_name, cpu_limit, mem_limit);
            if (code < 0)
            {
                // 服务器内部代码出现错误
                out_value["status"] = -2; // 未知错误---不暴露给用户
                out_value["reason"] = "发生了未知错误";
                // 序列化过程
                return;
            }
            else if (code > 0)
            {
                out_value["status"] = code; // 运行时报错，程序异常收到某个信号了
                out_value["reason"] = SignoToDesc(code);
                // 序列化过程
                return;
            }
            ***********************************************/
        }
    };
}
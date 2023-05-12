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

            if (code.size() == 0)
            {
                // TODO
            }

            // 形成的文件名只具有唯一性，没有目录和后缀名
            std::string file_name = FileUtil::UniqFileName();

            // 形成临时的代码文件
            FileUtil::WritweFile(PathUtil::Src(file_name), code);

            // 编译
            Compiler::Compile(file_name);

            // 运行
            Runner::Run(file_name, cpu_limit, mem_limit);
        }
    };
}
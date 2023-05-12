#pragma once

#include <iostream>
#include "compile.hpp"
#include "runner.hpp"

namespace ns_compile_and_run
{
    using namespace ns_compiler;
    using namespace ns_runner;
    class CompileAndRun
    {
    public:
        /***********************************************************
         * 输入:
         * code: 用户提交提交过来的代码
         * stdin: 用户有可能进行标准输入，可做扩展
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
         ***********************************************************/
        static void Start(const std::string &in_json, std::string *out_json)
        {
        }
    };
}
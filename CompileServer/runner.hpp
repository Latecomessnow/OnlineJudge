#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include "../Common/log.hpp"
#include "../Common/util.hpp"
namespace ns_runner
{
    using namespace ns_log;
    using namespace ns_util;
    class Runner
    {
    public:
        Runner(){}
        ~Runner(){}
    public:
        // 运行时只需要指定文件名，不需要带路径，带后缀
        static int Run(const std::string& file_name)
        {
            /******************************************
             * 程序运行
             * 1. 代码跑完，结果正确
             * 2. 代码跑完，结果不正确
             * 3. 代码没有跑完，出现异常
             * Run只需要考虑代码跑完，而不需要考虑结果正确与否
             * 结果是否正确应该用测试用例去决定
             * 我们只考虑代码是否正确运行完毕
             * 
             * 我们必须知道可执行程序是哪一个
             * 一个程序在默认启动的时候会有如下文件描述符
             * 标准输入: 不处理
             * 标准输出: 程序运行完成，结果是什么
             * 标准错误: 获取运行时错误的信息
            ********************************************/
           pid_t pid = fork();
           if (pid < 0)
           {

           }
           else if (pid == 0)
           {

           }
           else
           {

           }
        }
    };
}
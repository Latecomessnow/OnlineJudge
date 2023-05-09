#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../Common/util.hpp"
#include "../Common/log.hpp"

// 只负责进行代码的编译

namespace ns_compiler
{
    // 引入路径拼接功能
    using namespace ns_util;
    using namespace ns_log;
    class Compiler
    {
    public:
        Compiler() {}
        ~Compiler() {}
        // 输入参数，客户端发送过来的代码文件
        // 1. 编译成功: 返回true
        // 2. 编译失败: 返回false，将编译器编译出错的信息保存至临时文件，以便发送给用户
        // 临时文件xxx -> ./temp/xxx.cc，xxx -> ./temp/xxx.exe将临时文件都放进temp目录
        // file_name: 1234
        // 1234 -> ./temp/1234.cpp
        // 1234 -> ./temp/1234.exe
        // 1234 -> ./temp/1234.stderr
        static bool Compile(const std::string &file_name)
        {
            pid_t pid = fork();
            if (pid < 0)
            {
                // 子进程创建失败
                LOG(ERROR) << "系统内部出现错误，创建子进程失败"
                           << "\n";
                return false;
            }
            else if (pid == 0)
            {
                // 子进程有可能编译失败，将编译器生成的标准重定向到一个临时文件
                int stderr = open(PathUtil::Stderr(file_name).c_str(), O_CREAT | O_WRONLY, 0644);
                if (stderr < 0)
                {
                    LOG(WARNING) << "打开错误文件失败，没有形成stderr文件"
                                 << "\n";
                    exit(1);
                }

                // 重定向标准错误到stderr这个文件描述符
                dup2(stderr, 2);

                // 程序替换，并不影响进程的文件描述符表
                // 子进程调用编译器，完成对代码的编译
                // 程序替换，g++ -o target src -std=c++11
                execlp("g++", "-o", PathUtil::Exe(file_name).c_str(),
                       PathUtil::Src(file_name).c_str(), "-std=c++11", nullptr /*程序替换最后一个参数为空*/);
                // 程序替换一般不会失败，如果失败了走到这则说明没有成功的形成可执行文件
                LOG(ERROR) << "启动g++编译器失败，可能是参数出现了错误"
                           << "\n";
                exit(2);
            }
            else
            {
                // 父进程等待子进程
                waitpid(pid, nullptr, 0);
                // 编译是否成功，看有没有形成对应的可在执行程序
                if (FileUtil::IsFileExists(PathUtil::Exe(file_name)))
                {
                    LOG(INFO) << PathUtil::Src(file_name) << "编译成功!"
                              << "\n";
                    return true;
                }
            }
            LOG(ERROR) << "编译失败，没有形成可执行文件"
                       << "\n";
            return false;
        }
    };
}
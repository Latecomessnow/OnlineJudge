#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include "../Common/log.hpp"
#include "../Common/util.hpp"

namespace ns_runner
{
    using namespace ns_log;
    using namespace ns_util;
    class Runner
    {
    public:
        Runner() {}
        ~Runner() {}

    public:
        static void SetProcLimit(int _cpu_limit, int _mem_limit)
        {
            // 设置CPU时长
            struct rlimit cpu_limit;
            cpu_limit.rlim_max = RLIM_INFINITY;
            cpu_limit.rlim_cur = _cpu_limit;
            setrlimit(RLIMIT_CPU, &cpu_limit);

            // 设置内存大小
            struct rlimit mem_limit;
            mem_limit.rlim_max = RLIM_INFINITY;
            mem_limit.rlim_cur = _mem_limit * 1024; // 转化成为KB
            setrlimit(RLIMIT_AS, &mem_limit);
        }

        /********************************************
         * 返回值 > 0: 程序异常了，退出时收到了信号，返回值就是对应的信号编号
         * 返回值 == 0: 程序正确运行完毕，但结果不一定正确，结果放进了.stdout的临时文件
         * 返回值 < 0: 程序内部代码出现了错误
         * cpu_limit: 该程序运行时，可以使用的最大CPU上限
         * mem_limit: 该程序运行时，可以使用的最大内存大小(KB)
         *********************************************/
        // 运行时只需要指定文件名，不需要带路径，带后缀
        static int Run(const std::string &file_name, int cpu_limit, int mem_limit)
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

            // 形成运行时需要的临时文件
            std::string _execute = PathUtil::Exe(file_name);
            std::string _stdin = PathUtil::Stdin(file_name);
            std::string _stdout = PathUtil::Stdout(file_name);
            std::string _stderr = PathUtil::Stderr(file_name);

            // 打开文件描述符让子进程继承下去
            int _stdin_fd = open(_stdin.c_str(), O_CREAT | O_RDONLY, 0644);
            int _stdout_fd = open(_stdout.c_str(), O_CREAT | O_WRONLY, 0644);
            int _stderr_fd = open(_stderr.c_str(), O_CREAT | O_WRONLY, 0644);

            if (_stdin_fd < 0 || _stdout_fd < 0 || _stderr_fd < 0)
            {
                // 代表文件打开失败
                LOG(ERROR) << "运行时打开标准文件失败"
                           << "\n";
                return -1;
            }

            pid_t pid = fork();
            if (pid < 0)
            {
                // 关闭打开的文件描述符
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                LOG(ERROR) << "运行时创建子进程失败"
                           << "\n";
                return -2; // 代表创建子进程失败
            }
            else if (pid == 0)
            {
                // 子进程对标准输入、标准输出、标准错误进行重定向工作
                // 将原本显示到显示器上的数据重定向到特定文件
                dup2(_stdin_fd, 0);
                dup2(_stdout_fd, 1);
                dup2(_stderr_fd, 2);

                SetProcLimit(cpu_limit, mem_limit);

                // 此时我们已知可执行程序文件的完整路径_execute, 所以此处需要带路径的比较好
                // 而不是需要去环境变量里去找
                // /bin/ls
                execl(_execute.c_str() /*要执行程序的完整路径名*/, _execute.c_str() /*在命令行上该如何执行该程序*/, nullptr /*程序替换完成*/);
                exit(1);
            }
            else
            {
                // 父进程不需要这些文件描述符
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                int status = 0;
                waitpid(pid, &status, 0); // 此时父进程需要关心子进程的退出状态，才能知道子进程是否正确运行完
                // 我们需要知道程序的错误原因是什么，也就是子进程收到什么信号
                // 再Linux中，程序运行异常，一定是因为收到了信号
                LOG(INFO) << "运行完毕, info! 退出码: " << (status & 0x7F) << "\n";
                return status & 0x7F;
            }
        }
    };
}
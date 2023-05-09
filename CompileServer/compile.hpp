#pragma once

#include <iostream>
#include <unistd.h>
#include "../Common/util.hpp"

// 只负责进行代码的编译

namespace ns_compiler
{
    // 引入路径拼接功能
    using namespace ns_util;
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
            pid_t id = fork();
            if (id < 0)
            {
                return false;
            }
        }
    };
}
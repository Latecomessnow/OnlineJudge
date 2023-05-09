#pragma once

#include <iostream>
#include <string>

// 编译模块，运行模块都有可能会用上的添加后缀名...
namespace ns_util
{
    class PathUtil
    {
    public:
        // 构建源文件路径+后缀的完整文件名
        static std::string Src(const std::string &file_name)
        {
        }
        // 构建可执行程序的完整路径+后缀名
        static std::string Exe(const std::string &file_name)
        {
        }
        // 构建该程序对应的标准错误的完整路径+后缀名
        static std::string Stderr(const std::string &file_name)
        {
        }
    };
} // namespace ns_util
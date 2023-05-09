#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>

// 编译模块，运行模块都有可能会用上的添加后缀名...
namespace ns_util
{
    const std::string temp_path = "./temp/";
    // 文件路径相关的
    class PathUtil
    {
    public:
        // 123 -> ./temp/123.cpp
        static std::string AddSuffix(const std::string &file_name, const std::string &suffix)
        {
            std::string path_name = temp_path;
            path_name += file_name;
            path_name += suffix;
            return path_name;
        }
        // 构建源文件路径+后缀的完整文件名
        static std::string Src(const std::string &file_name)
        {
            return AddSuffix(file_name, ".cpp");
        }
        // 构建可执行程序的完整路径+后缀名
        static std::string Exe(const std::string &file_name)
        {
            return AddSuffix(file_name, ".exe");
        }
        // 构建该程序对应的标准错误的完整路径+后缀名
        static std::string Stderr(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stderr");
        }
    };

    // 文件相关的
    class FileUtil
    {
    public:
        static bool IsFileExists(const std::string &path_name)
        {
            struct stat st;
            if (stat(path_name.c_str(), &st) == 0)
            {
                // stat函数是获取某个路径下的文件属性，如果获取文件属性成功则返回0,表明文件存在
                return true;
            }
            return false;
        }
    };

    // 时间功能
    class TimeUtil
    {
    public:
        static std::string GetTimeStamp()
        {
            
        }
    };
} // namespace ns_util
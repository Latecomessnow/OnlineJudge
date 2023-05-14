#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <atomic>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

// 编译模块，运行模块都有可能会用上的添加后缀名...
namespace ns_util
{
    const std::string temp_path = "./temp/";

    // 时间功能
    class TimeUtil
    {
    public:
        static std::string GetTimeStamp()
        {
            // 调用gettimeofday获取时间戳
            struct timeval _time;
            gettimeofday(&_time, nullptr); // 设为nullptr表示要调用gettimeofday
            // 返回时间戳秒数
            return std::to_string(_time.tv_sec);
        }

        // 获取毫秒级时间戳
        static std::string GetTimeMs()
        {
            struct timeval _time;
            gettimeofday(&_time, nullptr);
            return std::to_string(_time.tv_sec * 1000 + _time.tv_usec / 1000);
        }
    };

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

        // 编译时需要的临时文件
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
        static std::string CompileError(const std::string &file_name)
        {
            return AddSuffix(file_name, ".compile_error");
        }

        // 运行时需要的临时文件
        static std::string Stdin(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stdin");
        }
        static std::string Stdout(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stdout");
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
        static std::string UniqFileName()
        {
            // 毫秒级时间戳+原子性递增唯一值: 确保文件名唯一性
            static std::atomic_uint id(0);
            // 原子性++
            id++;
            std::string ms = TimeUtil::GetTimeMs();
            std::string uniq_id = std::to_string(id);
            return ms + "_" + uniq_id;
        }
        static bool WriteFile(const std::string &target, const std::string &code)
        {
            std::ofstream out(target);
            if (!out.is_open())
                return false;
            out.write(code.c_str(), code.size());
            out.close();
            return true;
        }
        static bool ReadFile(const std::string &target, std::string *content, bool keep = false)
        {
            (*content).clear();
            std::ifstream in(target);
            if (!in.is_open())
                return false;
            std::string line;
            // getline按行读取不保存分隔符，如\n
            // getline内部重载了强制类型转换
            while (std::getline(in, line))
            {
                (*content) += line;
                (*content) += (keep ? "\n" : "");
            }
            in.close();
            return true;
        }
    };
} // namespace ns_util
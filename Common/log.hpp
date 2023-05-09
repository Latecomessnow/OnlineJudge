#pragma once

#include <iostream>
#include <string>
#include "util.hpp"

namespace ns_log
{
    using namespace ns_util;
    // 日志等级
    enum
    {
        INFO, // 就是整数
        DEBUG,
        WARNING,
        ERROR,
        FATAL
    };
    // 让日志返回标准输出流，以便于自定义输出
    inline std::ostream &Log(const std::string &level, const std::string &file_name, int line)
    {
        // 1. 添加日志等级
        std::string message = "[";
        message += level;
        message += "]";

        // 2. 添加报错文件名
        message += "[";
        message += file_name;
        message += "]";

        // 3. 添加报错行
        message += "[";
        message += std::to_string(line);
        message += "]";

        // 4. 添加日志时间
        message += "[";
        message += TimeUtil::GetTimeStamp();
        message += "]";
        return std::cout;
    }
    // Log() << "message"  // 带#号可以直接传过去的是字符，而不是整数了
    // 开放式日志接口
    #define LOG(level) Log(#level, __FILE__, __LINE__)
}
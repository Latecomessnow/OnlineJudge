#pragma once

#include <iostream>
#include <string>

// 根据题目list文件，加载所有题目信息到内存中
// model: 主要用来和数据进行交互，对外提供访问数据的接口
namespace ns_model
{
    struct Question
    {
        std::string number; // 题目编号，唯一
        std::string title;  // 题目的标题
        std::string desc;   // 题目的描述
        std::string star;   // 难度: 简单 中等 困难
        std::string header; // 题目预设给用户在代码编辑器的代码
        std::string tail; // 题目的测试用例，需要和header拼接，形成完整的编译代码
        int cpu_limit;      // 时间要求(S)
        int mem_limit;      // 空间要求(KB)
    };
    class Model
    {
    private:
        // 题号 : 题目信息
    };
}
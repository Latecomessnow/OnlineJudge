#pragma once

/*********************************MySQL版本******************************************/

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cassert>
#include <stdlib.h>
#include "../Common/util.hpp"
#include "../Common/log.hpp"

// 根据题目list文件，加载所有题目信息到内存中
// model: 主要用来和数据进行交互，对外提供访问数据的接口
namespace ns_model
{
    using namespace ns_util;
    using namespace ns_log;
    // 题目细节
    struct Question
    {
        std::string number; // 题目编号，唯一
        std::string title;  // 题目的标题
        std::string desc;   // 题目的描述
        std::string star;   // 难度: 简单 中等 困难
        std::string header; // 题目预设给用户在代码编辑器的代码
        std::string tail;   // 题目的测试用例，需要和header拼接，形成完整的编译代码
        int cpu_limit;      // 时间要求(S)
        int mem_limit;      // 空间要求(KB)
    };
    
    const std::string oj_questions = "oj_questions";

    class Model
    {
    public:
        Model() {}
        ~Model() {}
        bool QueryMySQL(const std::string &sql, std::vector<Question> *out)
        {
            
        }
        bool GetAllQuestions(std::vector<Question> *out)
        {
            std::string sql = "select * from ";
            sql += oj_questions;
            return QueryMySQL(sql, out);
        }
        bool GetOneQuestion(const std::string &number, Question *q)
        {
            bool res = false;
            std::string sql = "select * from ";
            sql += oj_questions;
            sql += " where number = ";
            sql += number;
            std::vector<Question> result;
            if (QueryMySQL(sql, &result))
            {
                if (result.size() == 1)
                {
                    *q = result[0];
                    res = true;
                }
            }
            return res;
        }
    };
}
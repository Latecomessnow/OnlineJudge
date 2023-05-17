#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "../Common/util.hpp"
#include <cassert>
#include <stdlib.h>

// 根据题目list文件，加载所有题目信息到内存中
// model: 主要用来和数据进行交互，对外提供访问数据的接口
namespace ns_model
{
    using namespace ns_util;
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

    const std::string questions_list = "./questions/questions/list";
    const std::string questions_path = "./questions";
    class Model
    {
    private:
        // 题号 : 题目细节
        std::unordered_map<std::string, Question> questions;

    public:
        Model()
        {
            assert(LoadQuestionsList(questions_list));
        }
        ~Model() {}
        bool LoadQuestionsList(const std::string &questions_list)
        {
            // 加载配置文件: questions/questions.list + 题目编号文件
            std::ifstream in(questions_list);
            if (!in.is_open()) return false;
            std::string line;
            // 从in这个流中按行读取到line中
            while (std::getline(in, line))
            {
                // 将经过切分后的字符串放进vector中
                std::vector<std::string> tokens;
                StringUtil::SplitString(line, &tokens, " ");
                // 1 判断回文数 简单 1 30000
                if (tokens.size() != 5) continue;
                Question q;
                q.number = tokens[0];
                q.title = tokens[1];
                q.star = tokens[2];
                q.cpu_limit = atoi(tokens[3].c_str());
                q.mem_limit = atoi(tokens[4].c_str());

                std::string question_number_path = questions_path;
                question_number_path += q.number;

                q.desc = ;
                q.header = ;
                q.tail = ;
            }
            in.close();
        }
        bool GetAllQuestions(std::vector<Question> *out)
        {
            if (questions.size() == 0)
                return false;
            for (const auto &q : questions)
                out->push_back(q.second);
            return true;
        }
        bool GetOneQuestion(const std::string &number, Question *q)
        {
            if (questions.size() == 0)
                return false;
            const std::unordered_map<std::string, Question>::iterator &iter = questions.find(number);
            // const auto &iter = questions.find(number);
            if (iter == questions.end())
                return false;
            (*q) = iter->second;
            return true;
        }
    };
}
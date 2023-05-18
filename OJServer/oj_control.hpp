#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "../Common/util.hpp"
#include "../Common/log.hpp"
#include "oj_model.hpp"

namespace ns_control
{
    using namespace ns_util;
    using namespace ns_log;
    using namespace ns_model;

    class Control
    {
    private:
        Model _model;
    public:
        Control(){}
        ~Control(){}
    public:
        // 根据题目数据构建网页
        // html: 输出型参数
        bool AllQuestions(std::string *html)
        {
            std::vector<struct Question> all;
            if (_model.GetAllQuestions(&all))
            {
                // 获取所有题目信息成功，将所有的题目构建成网页

            }
            else
            {

            }
        }
        bool Question(const std::string &number, std::string *html)
        {
            struct Question q;
            if (_model.GetOneQuestion(number, &q))
            {
                // 获取指定题目信息成功，将所有的题目构建成网页

            }
            else
            {

            }
        }
    };
}
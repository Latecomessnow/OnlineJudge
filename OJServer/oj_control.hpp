#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "../Common/util.hpp"
#include "../Common/log.hpp"
#include "oj_model.hpp"
#include "oj_view.hpp"

namespace ns_control
{
    using namespace ns_util;
    using namespace ns_log;
    using namespace ns_model;
    using namespace ns_view;
    class Control
    {
    private:
        Model _model; // 提供题目数据
        View _view;   // 将题目数据渲染成网页
    public:
        Control(){}
        ~Control(){}
    public:
        // 根据题目数据构建网页
        // html: 输出型参数
        bool AllQuestions(std::string *html)
        {
            bool ret = true;
            std::vector<struct Question> all;
            if (_model.GetAllQuestions(&all))
            {
                // 获取所有题目信息成功，将所有的题目构建成网页
                _view.AllExpandHtml(all, html);
            }
            else
            {
                *html = "获取题目列表失败";
                ret = false;
            }
            return ret;
        }
        bool Question(const std::string &number, std::string *html)
        {
            struct Question q;
            bool ret = true;
            if (_model.GetOneQuestion(number, &q))
            {
                // 获取指定题目信息成功，将所有的题目构建成网页
                _view.OneExpandHtml(q, html);
            }
            else
            {
                *html = "指定题目: " + number + "不存在!";
                ret = false;
            }
            return ret;
        }
    };
}
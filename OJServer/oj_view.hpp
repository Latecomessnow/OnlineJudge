#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "oj_model.hpp"
#include <ctemplate/template.h>
namespace ns_view
{
    using namespace ns_model;
    const std::string template_path = "./template_html/";
    class View
    {
    public:
        View(){}
        ~View(){}
    public:
        bool AllExpandHtml(const std::vector<struct Question> &questions, std::string *html)
        {
            // 题目的编号 题目的标题 题目的难度
            // 推荐使用表格显示
            // 1. 形成待渲染文件的路径
            std::string src_html = template_path + "all_questions.html";
            // 2. 形成字典
            ctemplate::TemplateDictionary root("all_questions");
            for (const auto& q : questions)
            {
                // 添加子字典
                ctemplate::TemplateDictionary *sub = root.AddSectionDictionary("questions_list");
                sub->SetValue("number", q.number);
                sub->SetValue("title", q.title);
                sub->SetValue("star", q.star);
            }
            
            // 3. 获取被渲染的html                                                    // 不对html做任何的删除
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP)

            // 4. 开始完成渲染功能
            tpl->Expand(html, &root);
            return true;
        }
        bool OneExpandHtml(const struct Question &q, std::string *html)
        {
            return true;
        }
    };
}
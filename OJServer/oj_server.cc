#include <iostream>
#include <string>
#include <signal.h>
#include "../Common/httplib.h"
#include "oj_control.hpp"

using namespace httplib;
using namespace ns_control;

static Control *ctrl_ptr = nullptr;

void Recovery(int signo)
{
    ctrl_ptr->RecoveryMachine();
}

int main()
{
    signal(SIGQUIT, Recovery);

    // 用户请求的服务路由功能
    Server svr;

    // control 负责业务逻辑
    Control ctrl;
    ctrl_ptr = &ctrl;

    // 1. 用户获取所有的题目列表
    svr.Get("/all_questions", [&ctrl](const Request &req, Response &resp)
            {
                // 返回一张包含所有题目列表的html网页
                std::string html;
                ctrl.AllQuestions(&html);
                resp.set_content(html, "text/html; charset=utf-8");
                
                // 设置响应内容
                // resp.set_content("这是所有题目的列表", "text/plain; charset=utf-8");
            });

    // 2，用户根据题目编号获取特定题目内容
    // /questions/100 正则匹配: \d代表的是匹配的是数字，+代表的是一个或多个
    // R"()"->原始字符raw string, 保持字符串原貌，不用做相关的转义
    svr.Get(R"(/question/(\d+))", [&ctrl](const Request &req, Response &resp)
            {
                std::string number = req.matches[1]; // 100
                std::string html;
                // 构建特定题目的网页信息
                ctrl.Question(number, &html);
                resp.set_content(html, "text/html; charset=utf-8");

                // resp.set_content("这是指定的一道题: " + number, "text/plain; charset=utf-8");
            });

    // // 3. 用户提交代码，使用我们的判题功能(1. 每道题的测试用例 2. 调用compile_and_run)
    // svr.Get(R"(/judge/(\d+))", [&ctrl](const Request &req, Response &resp)
    //         {
    //             std::string number = req.matches[1];
    //             std::string result_json;
    //             ctrl.Judge(number, req.body, &result_json);
    //             resp.set_content(result_json, "application/json;charset=utf-8");
    //             // resp.set_content("指定题目的判题: " + number, "text/plain; charset=utf-8");
    //         });

    // 3. 用户提交代码，使用我们的判题功能(1. 每道题的测试用例 2. 调用compile_and_run)
    // 使用http的post方法去请求
    svr.Post(R"(/judge/(\d+))", [&ctrl](const Request &req, Response &resp)
            {
                std::string number = req.matches[1];
                std::string result_json;
                ctrl.Judge(number, req.body, &result_json);
                resp.set_content(result_json, "application/json;charset=utf-8");
                // resp.set_content("指定题目的判题: " + number, "text/plain; charset=utf-8");
            });

    svr.set_base_dir("./wwwroot");

    svr.listen("0.0.0.0", 8080);
    return 0;
}
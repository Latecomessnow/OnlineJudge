#include <iostream>
#include "../Common/httplib.h"

using namespace httplib;

int main()
{
    // 用户请求的服务路由功能
    Server svr;

    // 1. 用户获取所有的题目列表
    svr.Get("/all_questions", [](const Request &req, Response &resp){
        // 设置响应内容
        resp.set_content("这是所有题目的列表", "text/plain; charset=utf-8"); 
    });

    // 2，用户根据题目编号获取特定题目内容
    // /questions/100 正则匹配: \d代表的是匹配的是数字，+代表的是一个或多个
    // R"()"->原始字符raw string, 保持字符串原貌，不用做相关的转义
    svr.Get(R"(/questions/(\d+))", [](const Request &req, Response &resp){
        std::string number = req.matches[1]; // 100
        resp.set_content("这是指定的一道题: " + number, "text/plain; charset=utf-8");
    });

    // 3. 用户提交代码，使用我们的判题功能(1. 每道题的测试用例 2. 调用compile_and_run)
    svr.Get(R"(/judge/(\d+))", [](const Request &req, Response &resp){
        std::string number = req.matches[1];
        resp.set_content("指定题目的判题: " + number, "text/plain; charset=utf-8");
    });

    svr.set_base_dir("./wwwroot");

    svr.listen("0.0.0.0", 8080);
    return 0;
}
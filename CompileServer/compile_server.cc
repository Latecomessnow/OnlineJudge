#include "compile_run.hpp"
#include "../Common/httplib.h"

using namespace ns_compile_and_run;
using namespace httplib;

void Usage(std::string proc)
{
    std::cerr << "Usage: " << "\n\t" << proc << " port\n" << std::endl;
}

// 启动编译服务，./compile_server 8081 ./compile_server 8082 ./compile_server 8083...
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        return 1;
    }
    Server svr;

    svr.Get("/hello", [](const Request &req, Response &resp){
        // 测试
        resp.set_content("hello http, 你好 http!", "text/plain;charset=utf-8");
    });

    svr.Post("/compile_and_run", [](const Request &req, Response & resp){
        // 用户请求的服务正文正是我们想要的json string
        std::string in_json = req.body; // 请求正文
        std::string out_json;
        if (!in_json.empty())
        {
            CompileAndRun::Start(in_json, &out_json);
            resp.set_content(out_json, "application/json;charset=utf-8");
        }
    });

    svr.set_base_dir("../wwwroot");
    svr.listen("0.0.0.0", atoi(argv[1]));

    return 0;
}

///////////////////////////////////////////////////////////////////////////////

// // 编译服务随时可能被多个人请求，必须保证传递上来的code，形成源文件名称的时候，要具有
// // 唯一性，要不然多个用户之间会互相影响
// // ./compile_server port

// int main()
// {
//     // 提供编译服务，打包形成一个网络服务
//     // 引入第三方开源库, cpp-httplib

//     // 通过http让client给我们上传一个json string
//     // in_json: {"code": "#include...", "input": "","cpu_limit":1, "mem_limit":10240}
//     // out_json: {"status":"0", "reason":"","stdout":"","stderr":"",}
//     // 充当客户端请求的json串
//     std::string in_json;
//     Json::Value in_value;

//     // C++较新的特性，语法: R"(string)"，string当中的特殊字符都保持原样不变 raw string
//     // in_value["code"] = "";
//     // in_value["code"] = R"(#include <iostream>\nint main()\n{\n std::cout << "Test Hello" << std::endl;\nreturn 0;\n})";
//     in_value["code"] = R"(
//         #include <iostream>
//         int main()
//         {
//             std::cout << "Test Hello" << std::endl;
//             return 0;
//         })";
//     in_value["input"] = "";
//     in_value["cpu_limit"] = 1;
//     in_value["mem_limit"] = 10240 * 3; // 30M

//     Json::StyledWriter writer;
//     // Json::FastWriter writer;
//     in_json = writer.write(in_value);

//     std::cout << in_json << std::endl;

//     // 这个是将来给客户端返回的结果串
//     std::string out_json;
//     CompileAndRun::Start(in_json, &out_json); // 模拟传入一个json字符串看是否能够正常编译

//     std::cout << out_json << std::endl;
//     return 0;
// }
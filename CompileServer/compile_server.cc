#include "compile_run.hpp"

using namespace ns_compile_and_run;

// 编译服务随时可能被多个人请求，必须保证传递上来的code，形成源文件名称的时候，要具有
// 唯一性，要不然多个用户之间会互相影响
// ./compile_server port
int main()
{
    // 通过http让client给我们上传一个json string
    // in_json: {"code": "#include...", "input": "","cpu_limit":1, "mem_limit":10240}
    // out_json: {"status":"0", "reason":"","stdout":"","stderr":"",}
    // 充当客户端请求的json串
    std::string in_json;
    Json::Value in_value;

    // C++较新的特性，语法: R"(string)"，string当中的特殊字符都保持原样不变 raw string
    // in_value["code"] = "";
    // in_value["code"] = R"(#include <iostream>\nint main()\n{\n std::cout << "Test Hello" << std::endl;\nreturn 0;\n})";
    in_value["code"] = R"(
        #include <iostream>
        int main()
        {
            std::cout << "Test Hello" << std::endl;
            while (1);
            return 0;
        })";
    in_value["input"] = "";
    in_value["cpu_limit"] = 1;
    in_value["mem_limit"] = 10240 * 3; // 30M

    // Json::StyledWriter writer;
    Json::FastWriter writer;
    in_json = writer.write(in_value);

    std::cout << in_json << std::endl;

    // 这个是将来给客户端返回的结果串
    std::string out_json;
    CompileAndRun::Start(in_json, &out_json); // 模拟传入一个json字符串看是否能够正常编译

    std::cout << out_json << std::endl;
    return 0;
}
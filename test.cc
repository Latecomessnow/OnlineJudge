#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
#include <cstdlib>

#include <jsoncpp/json/json.h>

int main()
{
    // 序列化工作，将结构化的数据转为一个字符串
    // Value是Json的中间类，可以填充KV值
    Json::Value root;
    root["left"] = "左边";
    root["age"] = "年龄";
    root["hello"] = "你好";

    // Json::StyledWriter writer;
    Json::FastWriter writer;
    std::string str = writer.write(root);
    std::cout << str << std::endl;
    //std::cout << writer.write(root) << std::endl;
    return 0;
}

// void handler(int signo)
// {
//     std::cout << "signo: " << signo << std::endl;
//     exit(1);
// }

// int main()
// {
//     // 资源不足，导致OS终止进程，是通过信号终止的

//     // 捕捉信号
//     for (int i = 1; i <= 31; i++)
//     {
//         signal(i, handler);
//     }

//     // 会收到24号信号SIGXCPU，占用CPU时间过长
//     // 限制程序运行时长
//     // struct rlimit r;
//     // r.rlim_cur = 1; // 限制占用CPU秒数
//     // r.rlim_max = RLIM_INFINITY;
//     // setrlimit(RLIMIT_CPU, &r);
//     // while (1);

//     // 限制申请的内存空间
//     struct rlimit r;
//     r.rlim_cur = 1024 * 1024 * 40;
//     r.rlim_max = RLIM_INFINITY;
//     setrlimit(RLIMIT_AS, &r);
//     int count = 0;
//     // 会收到6号信号SIGABRT，内存申请失败, 抛异常
//     while (1)
//     {
//         int *p = new int[1024 * 1024];
//         count++;
//         std::cout << "count: " << count << std::endl;
//         sleep(1);
//     }
//     return 0;
// }
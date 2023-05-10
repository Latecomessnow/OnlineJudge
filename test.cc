#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
#include <cstdlib>

void handler(int signo)
{
    std::cout << "signo: " << signo << std::endl;
    exit(1);
}

int main()
{
    // 资源不足，导致OS终止进程，是通过信号终止的

    // 捕捉信号
    for (int i = 1; i <= 31; i++)
    {
        signal(i, handler);
    }
    // 限制程序运行时长
    // struct rlimit r;
    // r.rlim_cur = 1; // 限制占用CPU秒数
    // r.rlim_max = RLIM_INFINITY;
    // setrlimit(RLIMIT_CPU, &r);
    // while (1);

    // 限制申请的内存空间
    struct rlimit r;
    r.rlim_cur = 1024 * 1024 * 40;
    r.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_AS, &r);
    int count = 0;
    while (1)
    {
        int *p = new int[1024 * 1024];
        count++;
        std::cout << "count: " << count << std::endl;
        sleep(1);
    }
    return 0;
}
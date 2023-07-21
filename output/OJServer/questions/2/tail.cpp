// 最后header和tail会被拼接在一块，再进行#include "header.cpp"则会报错
// 所以可在编译运行时的程序替换出用-D选项进行宏定义去掉这部分的代码
#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1()
{
    vector<int> v = {1, 2, 3, 4, 5, 6, 12, 3, 4, -1};
    int ans = Solution().Max(v);
    if (ans == 12) std::cout << "通过测试用例1" << std::endl;
    else std::cout << "未通过测试用例1" << std::endl;
}
void Test2()
{
    vector<int> v = {-1, -2, -3, -4, -5, -6, -12, -3, -4, -1};
    int ans = Solution().Max(v);
    if (ans == -1) std::cout << "通过测试用例1" << std::endl;
    else std::cout << "未通过测试用例1" << std::endl;
}
int main()
{
    Test1();
    Test2();
    return 0;
}
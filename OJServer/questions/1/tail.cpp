#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1()
{
    bool ret = Solution().isPalindrome(121);
    if (ret) std::cout << "通过测试用例1, 测试121通过 ... OK!" << std::endl;
    // else std::cout << "未通过测试用例1, 用例1测试值是121" << std::endl;
    else std::cout << "Wrong Answer!" << std::endl;
}

void Test2()
{
    bool ret = Solution().isPalindrome(-121);
    if (ret) std::cout << "通过测试用例2, 测试-121通过 ... OK!" << std::endl;
    // else std::cout << "未通过测试用例1, 用例1测试值是-121" << std::endl;
    else std::cout << "Wrong Answer!" << std::endl;
}

int main()
{
    Test1();
    Test2();
    return 0;
}
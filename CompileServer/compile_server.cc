#include <iostream>
#include "compile.hpp"

using namespace ns_compiler;

int main()
{
    std::string code = "code";
    Compiler::Compile(code);
    return 0;
}
#include <iostream>
#include "compile.hpp"
#include "runner.hpp"

using namespace ns_compiler;
using namespace ns_runner;

int main()
{
    std::string code = "code";
    Compiler::Compile(code);
    Runner::Run(code);
    return 0;
}
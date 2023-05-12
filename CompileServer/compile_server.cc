#include "compile_run.hpp"

using namespace ns_compile_and_run;

int main()
{
    std::string code = "code";
    Compiler::Compile(code);
    Runner::Run(code);
    return 0;
}
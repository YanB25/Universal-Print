#include <iostream>
#include "helloworld/argparser.hpp"
using namespace hello;
int main()
{
    auto& parser = argparser::init("Simple Commandline tool");
    parser.print_promt();

}
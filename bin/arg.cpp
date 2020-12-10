#include <iostream>

#include "helloworld/argparser.hpp"
using namespace hello;
int main(int argc, char *argv[])
{
    auto &parser = argparser::init("Simple Commandline tool");
    uint64_t time;
    std::string name;
    parser.flag(&time, "--time", "-t", "The time to test", "4");
    parser.flag(&name, "--name", "-n", "The name", "default");
    parser.apply(argc, argv);
    parser.print_promt();
    std::cout << "time is " << time << std::endl;
    std::cout << "name is " << name << std::endl;
}
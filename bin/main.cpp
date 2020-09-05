#include <iostream>

#include "helloworld/debug.hpp"
#include "helloworld/helloworld.hpp"
int dead_loop()
{
    while (true)
    {
    }
    return 0;
}
int main()
{
    std::cout << hello::hello() << std::endl;
    info("It's an info.");
    dinfo("It's an debug info.");
    info_if(false,
            "It will not cause a dead loop, because it's not evaluated. %d",
            dead_loop());
    check(true, "It's a check");
    dcheck(true, "It's a debug check");
    return 0;
}
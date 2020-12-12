#include <iostream>
#include <memory>

#include "helloworld/debug.hpp"
#include "helloworld/helloworld.hpp"
#include "helloworld/cxxdebug.hpp"
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

    LOG(INFO) << "!!!";
    LOG(INFO);
    LOG(WARNING) << "hei!";
    LOG(ERROR) << "hei!";
    LOG_IF(INFO, true) << "true!";
    LOG_IF(INFO, false) << dead_loop();
    std::shared_ptr<int> p;
    LOG_IF(INFO, p) << "pointer !";
    p = std::make_shared<int>(5);
    LOG_IF(INFO, p) << "pointer again!";
    for (int i = 0; i < 100; ++i)
    {
        LOG_EVERY_N(INFO, 20) << "Log every 20!";
        LOG_EVERY_N(INFO, 20) << "Log every again 20!";
        LOG_EVERY_N(INFO, 20) << "Log every hei 20!";
        LOG_IF_EVERY_N(INFO, true, 20) << "IF: Log every hei 20!";
        LOG_IF_EVERY_N(INFO, false, 20) << "false: Log every hei 20!";
        LOG_FIRST_N(INFO, 3) << "only first 3";
    }

    DLOG(INFO) << "Dlog info";
    DLOG(WARNING) << "Dlog info";
    DLOG(ERROR) << "Dlog info";
    int* pn = nullptr;
    DLOG(INFO) << "It's " << *pn;
    DLOG(INFO) << "It's " << dead_loop();

    CHECK_EQ(1, 2) << "It must be false";
    CHECK_EQ(1, 1) << "????";

    CHECK_NE(4, 4);
    CHECK_NE(4, 5) << "??????";

    CHECK_GT(4, 5);
    CHECK_GT(5, 4) << "???";

    CHECK_LT(5, 2);
    CHECK_LT(2, 5) << "???";

    CHECK_GE(2, 3);
    CHECK_GE(3, 3) << "???";

    CHECK_LE(3, 2);
    CHECK_LE(3, 3) << "???";

    CHECK(true) << "???";
    CHECK(false) << "So it fails";

    DCHECK(false);
    DLOG(ERROR) << "In Release, you should not see me";

    std::string my_name = "yanbin";
    SHOW(my_name);
    return 0;
}
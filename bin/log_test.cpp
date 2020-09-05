#include "helloworld/debug.hpp"

int main()
{
    info();
    warn();
    error();

    dinfo();
    dwarn();
    derror();

    info_if(true);
    warn_if(true);
    error_if(true);

    dinfo_if(true);
    dwarn_if(true);
    derror_if(true);

    info("It's an info");
    warn("It's an warn");
    error("It's an error");

    dinfo("It's an info");
    dwarn("It's an warn");
    derror("It's an error");

    info_if(true, "It's an info");
    warn_if(true, "It's an warn");
    error_if(true, "It's an error");

    dinfo_if(true, "It's an info");
    dwarn_if(true, "It's an warn");
    derror_if(true, "It's an error");

    info("It's an info: %d", 0);
    warn("It's an warn: %d", 1);
    error("It's an error: %d", 2);

    dinfo("It's an info: %d", 0);
    dwarn("It's an warn: %d", 1);
    derror("It's an error: %d", 2);

    info_if(true, "It's an info: %d", 0);
    warn_if(true, "It's an warn: %d", 1);
    error_if(true, "It's an error: %d", 2);

    dinfo_if(true, "It's an info: %d", 0);
    dwarn_if(true, "It's an warn: %d", 1);
    derror_if(true, "It's an error: %d", 2);

    check(true);
    check(true, "hello world");
    check(true, "hello world: %d", 0);

    dcheck(true);
    dcheck(true, "hello world");
    dcheck(true, "hello world: %d", 0);
}
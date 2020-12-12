#ifndef DEBUG_H
#define DEBUG_H
#include <ostream>
#include <list>
#include <string>
std::ostream& operator<<(std::ostream& os, std::list<std::pair<std::string, std::string>> pairs)
{
    if (!pairs.empty())
    {
        os << "[";
        for (const auto&[first, second]: pairs)
        {
            os << "{" << first << ", " << second << "}, ";
        }
        os << "]" << std::endl;
    }
    else
    {
        os << "[]" << std::endl;
    }
    return os;
}
#endif
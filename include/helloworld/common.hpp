#ifndef COMMON_H
#define COMMON_H
#include <iostream>
#include <vector>
#include <string>

// TODO: make it portable.
// #if !define __builtin_expect
// #define __builtin_expect(x, expected_value) (x)
// #pragma message("No __builtin_expect defined.")
// #endif

#if !defined(likely)
#define likely(x) (__builtin_expect(!!(x), 1))
#endif

#if !defined(unlikely)
#define unlikely(x) (__builtin_expect(!!(x), 0))
#endif

namespace hello
{
namespace debug
{
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        os << vec[i];
        if (i != vec.size() - 1)
        {
            os << ", ";
        }
    }
    os << "]";
    return os;
}
std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& vec)
{
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        os << "\"" << vec[i] << "\"";
        if (i != vec.size() - 1)
        {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

}
}


#endif
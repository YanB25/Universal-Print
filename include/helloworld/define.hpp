#ifndef HELLOWORLD_DEFINE_H
#define HELLOWORLD_DEFINE_H
#include <stdint.h>
namespace hello
{
namespace literals
{
constexpr uint64_t operator""_K(unsigned long long i)
{
    return (uint64_t) i * 1000;
}
constexpr uint64_t operator""_KB(unsigned long long i)
{
    return (uint64_t) i * 1024;
}
constexpr uint64_t operator""_M(unsigned long long i)
{
    return (uint64_t) i * 1000 * 1_K;
}
constexpr uint64_t operator""_MB(unsigned long long i)
{
    return (uint64_t) i * 1024 * 1_KB;
}
constexpr uint64_t operator""_G(unsigned long long i)
{
    return (uint64_t) i * 1000 * 1_M;
}
constexpr uint64_t operator""_GB(unsigned long long i)
{
    return (uint64_t) i * 1024 * 1_MB;
}
constexpr uint64_t operator""_T(unsigned long long i)
{
    return (uint64_t) i * 1000 * 1_G;
}
constexpr uint64_t operator""_TB(unsigned long long i)
{
    return (uint64_t) i * 1024 * 1_GB;
}
constexpr uint64_t operator""_P(unsigned long long i)
{
    return (uint64_t) i * 1000 * 1_T;
}
constexpr uint64_t operator""_PB(unsigned long long i)
{
    return (uint64_t) i * 1024 * 1_TB;
}
}  // namespace literals

}  // namespace hello
#endif
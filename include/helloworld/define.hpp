#ifndef DEFINE_H
#define DEFINE_H
#include <stdint.h>
namespace hello
{
namespace define
{
constexpr uint64_t KB = 1024ull;
constexpr uint64_t MB = 1024ull * KB;
constexpr uint64_t GB = 1024ull * MB;
constexpr uint64_t TB = 1024ull * GB;

constexpr uint64_t K = 1000ull;
constexpr uint64_t M = 1000ull * K;
constexpr uint64_t G = 1000ull * M;
constexpr uint64_t T = 1000ull * G;

constexpr uint64_t Ki = 1024ull;
constexpr uint64_t Mi = 1024ull * Ki;
constexpr uint64_t Gi = 1024ull * Mi;
constexpr uint64_t Ti = 1024ull * Gi;
}  // namespace define
}  // namespace hello
#endif
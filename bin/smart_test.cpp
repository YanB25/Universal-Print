#include "helloworld/smart.hpp"

#include "helloworld/debug.hpp"
constexpr static uint64_t k5 = 5ull * 1024;
constexpr static uint64_t dump = 380ull;
constexpr static uint64_t m7 = 7ull * 1024 * 1024;
constexpr static uint64_t g4 = 4ull * 1024 * 1024 * 1024;
constexpr static uint64_t t2 = 2ull * 1024 * 1024 * 1024 * 1024;

constexpr static uint64_t dump_ops = 482ull;
constexpr static uint64_t k3_ops = 3ull * 1000;
constexpr static uint64_t m2_ops = 2ull * 1000 * 1000;
constexpr static uint64_t g4_ops = 4ull * 1000 * 1000 * 1000;
constexpr static uint64_t t1_ops = 1ull * 1000 * 1000 * 1000 * 1000;

constexpr static uint64_t dump_ns = 518ull;
constexpr static uint64_t us_8 = 8ull * 1000;
constexpr static uint64_t ms_142 = 142ull * 1000 * 1000;
constexpr static uint64_t s_42 = 42ull * 1000 * 1000 * 1000;

int main()
{
    info("%s", hello::smart::toSize(dump).c_str());
    info("%s", hello::smart::toSize(k5).c_str());
    info("%s", hello::smart::toSize(m7).c_str());
    info("%s", hello::smart::toSize(g4).c_str());
    info("%s", hello::smart::toSize(t2).c_str());

    info("%s", hello::smart::toOps(dump_ops).c_str());
    info("%s", hello::smart::toOps(k3_ops).c_str());
    info("%s", hello::smart::toOps(m2_ops).c_str());
    info("%s", hello::smart::toOps(g4_ops).c_str());
    info("%s", hello::smart::toOps(t1_ops).c_str());

    info("%s", hello::smart::nsToLatency(dump_ns).c_str());
    info("%s", hello::smart::nsToLatency(us_8).c_str());
    info("%s", hello::smart::nsToLatency(ms_142).c_str());
    info("%s", hello::smart::nsToLatency(s_42).c_str());
    return 0;
}
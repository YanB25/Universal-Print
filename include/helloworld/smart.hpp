#ifndef SMART_H
#define SMART_H
#include <string>

#include "define.hpp"
namespace hello
{
namespace smart
{
inline std::string toSize(double size)
{
    if (size < 1 * define::KB)
    {
        return std::to_string(size) + " B";
    }
    else if (size < 1 * define::MB)
    {
        return std::to_string(size / define::KB) + " KB";
    }
    else if (size < 1 * define::GB)
    {
        return std::to_string(size / define::MB) + " MB";
    }
    else if (size < 1 * define::TB)
    {
        return std::to_string(size / define::GB) + " GB";
    }
    else
    {
        return std::to_string(size / define::TB) + " TB";
    }
}
inline std::string toNum(double ops)
{
    if (ops < 1 * define::K)
    {
        return std::to_string(ops) + " ";
    }
    else if (ops < 1 * define::M)
    {
        return std::to_string(ops / define::K) + " K";
    }
    else if (ops < 1 * define::G)
    {
        return std::to_string(ops / define::M) + " M";
    }
    else if (ops < 1 * define::T)
    {
        return std::to_string(ops / define::G) + " G";
    }
    else
    {
        return std::to_string(ops / define::T) + " T";
    }
}
inline std::string toOps(double ops)
{
    return toNum(ops) + "ops";
}

inline std::string nsToLatency(double ns)
{
    if (ns < define::K)
    {
        return std::to_string(ns) + " ns";
    }
    else if (ns < define::M)
    {
        return std::to_string(ns / define::K) + " us";
    }
    else if (ns < define::G)
    {
        return std::to_string(ns / define::M) + " ms";
    }
    return std::to_string(ns / define::G) + " s";
}
}  // namespace smart
}  // namespace hello
#endif
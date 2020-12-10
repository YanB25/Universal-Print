#ifndef ARGPARSER_COMMON_H
#define ARGPARSER_COMMON_H
#include <cctype>
#include <string>
namespace hello
{
namespace argparser
{
namespace flag
{
inline static bool is_flag(const std::string &str)
{
    return !str.empty() && str[0] == '-';
}
inline static bool is_full_flag(const std::string &name)
{
    return name.size() >= 3 && name[0] == '-' && name[1] == '-' &&
           isalpha(name[2]);
}
inline static bool is_short_flag(const std::string &name)
{
    return name.empty() ||
           (name.size() >= 2 && name[0] == '-' && isalpha(name[1]));
}

}  // namespace flag
}  // namespace argparser
}  // namespace hello
#endif
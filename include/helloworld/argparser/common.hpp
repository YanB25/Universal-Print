#ifndef ARG_PARSER_COMMON_H
#define ARG_PARSER_COMMON_H
#include <cctype>
#include <string>
namespace argparser
{
namespace flag
{
inline static bool is_full_flag(const std::string &name)
{
    return name.size() >= 3 && name[0] == '-' && name[1] == '-' &&
           isalpha(name[2]);
}
inline static bool is_short_flag(const std::string &name)
{
    return name.size() >= 2 && name[0] == '-' && isalpha(name[1]);
}
inline static bool is_flag(const std::string &str)
{
    return is_full_flag(str) || is_short_flag(str);
}
std::vector<std::string> split(std::string str,
                               const std::string& delimiter)
{
    std::vector<std::string> ret;
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos)
    {
        token = str.substr(0, pos);
        ret.emplace_back(std::move(token));
        str.erase(0, pos + delimiter.length());
    }
    ret.push_back(std::move(str));
    return ret;
}

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

}  // namespace flag
}  // namespace argparser
#endif
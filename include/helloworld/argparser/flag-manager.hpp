#ifndef ARG_PARSER_MANAGER_H
#define ARG_PARSER_MANAGER_H

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "./common.hpp"
#include "./flag.hpp"

namespace hello
{
namespace argparser
{
namespace flag
{
class FlagManager
{
public:
    using Pointer = std::shared_ptr<FlagManager>;
    static Pointer new_instance()
    {
        return std::make_unique<FlagManager>();
    }
    template <typename T>
    bool add_flag(T *slot,
                  const std::string &full_name,
                  const std::string &short_name,
                  const std::string &desc,
                  const std::optional<std::string> &default_val,
                  bool required)
    {
        if (!is_full_flag(full_name))
        {
            std::cerr << "Failed to register flag " << full_name
                      << ": identity not allowed" << std::endl;
            return false;
        }
        if (!is_short_flag(short_name))
        {
            std::cerr << "Failed to register flag " << short_name << "("
                      << full_name << ")"
                      << ": identity not allowed" << std::endl;
            return false;
        }
        if (!unique_full_flag(full_name))
        {
            std::cerr << "Failed to register flag " << full_name
                      << ": flag already registered" << std::endl;
            return false;
        }
        if (!unique_short_flag(full_name))
        {
            std::cerr << "Failed to register flag " << short_name << "("
                      << full_name << ")"
                      << ": flag already registered" << std::endl;
            return false;
        }
        flags_.emplace_back(flag::ConcreteFlag<T>::make_flag(
            slot, full_name, short_name, desc, default_val));
        required_.emplace_back(required);
        applied_.emplace_back(false);
        if (default_val.has_value())
        {
            if (!flags_.back()->apply_default())
            {
                std::cerr << "Failed to register flag " << full_name << ": "
                          << "default value \"" << default_val.value()
                          << "\" not parsable" << std::endl;
                return false;
            }
        }
        max_full_name_len_ = std::max(max_full_name_len_, full_name.size());
        max_short_name_len_ = std::max(max_short_name_len_, short_name.size());
        /**
         * We allow a flag which does not has a full_name OR a short_name.
         * In this case, do not insert "" into the map
         */
        if (!full_name.empty())
        {
            registered_full_flags_.insert(full_name);
        }
        if (!short_name.empty())
        {
            registered_short_flags_.insert(short_name);
        }
        return true;
    }
    bool empty() const
    {
        return flags_.empty();
    }
    bool apply(const std::string &key, const std::string &value)
    {
        for (size_t i = 0; i < flags_.size(); ++i)
        {
            const auto &flag = flags_[i];
            if (flag->match(key))
            {
                if (!flag->apply(value))
                {
                    std::cerr << "Failed to apply " << key << "=\"" << value
                              << "\": \"" << value << "\" not parsable"
                              << std::endl;
                    return false;
                }
                applied_[i] = true;
                return true;
            }
        }
        std::cerr << "Failed to apply " << key << "=\"" << value << "\": "
                  << "Flag not found" << std::endl;
        return false;
    }
    bool contain(const std::string name) const
    {
        for (const auto &flag : flags_)
        {
            if (flag->match(name))
            {
                return true;
            }
        }
        return false;
    }
    size_t size() const
    {
        return flags_.size();
    }
    const std::vector<std::shared_ptr<flag::Flag>> &flags() const
    {
        return flags_;
    }
    using FlagId = std::tuple<std::string, std::string>;
    std::vector<FlagId> missing_keys() const
    {
        std::vector<FlagId> ret;
        for (size_t i = 0; i < required_.size(); ++i)
        {
            if (required_[i] && !applied_[i])
            {
                const auto &flag = flags_[i];
                ret.emplace_back(flag->full_name(), flag->short_name());
            }
        }
        return ret;
    }
    // TODO: make it formator
    void print_flags() const
    {
        if (empty())
        {
            return;
        }
        std::cout << "Flags:" << std::endl;
        for (const auto &flag : flags_)
        {
            auto short_name = flag->short_name();
            auto full_name = flag->full_name();
            std::cout << "  ";
            std::cout << std::string(max_short_name_len_ - short_name.size(),
                                     ' ');
            std::cout << short_name << (short_name.empty() ? "  " : ", ");
            std::cout << full_name;
            std::cout << std::endl;
            size_t padding_len =
                2 + max_short_name_len_ + 2 + max_full_name_len_ + 2;
            auto padding_str = std::string(padding_len, ' ');
            std::cout << padding_str;
            auto desc = flag->desc();
            size_t pos = 0;
            std::string token;
            size_t current_column = 0;
            while ((pos = desc.find(" ")) != std::string::npos)
            {
                token = desc.substr(0, pos);
                current_column += token.size();
                if (current_column >= 80)
                {
                    current_column = 0;
                    std::cout << std::endl << padding_str;
                }
                std::cout << token << " ";
                desc.erase(0, pos + 1);
            }
            std::cout << desc;
            std::cout << std::endl;
        }
    }

    FlagManager() = default;

    ~FlagManager() = default;

private:
    bool unique_full_flag(const std::string &name) const
    {
        return registered_full_flags_.find(name) ==
               registered_full_flags_.end();
        ;
    }
    bool unique_short_flag(const std::string &name) const
    {
        return registered_short_flags_.find(name) ==
               registered_short_flags_.end();
        ;
    }

    std::set<std::string> registered_full_flags_;
    std::set<std::string> registered_short_flags_;
    std::vector<std::shared_ptr<flag::Flag>> flags_;

    std::vector<bool> required_;
    std::vector<bool> applied_;

    size_t max_full_name_len_{0};
    size_t max_short_name_len_{0};
};
}  // namespace flag
}  // namespace argparser
}  // namespace hello
#endif
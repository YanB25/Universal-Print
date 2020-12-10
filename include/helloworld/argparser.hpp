#ifndef ARG_PARSER_H
#define ARG_PARSER_H
#include <cctype>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>
namespace hello
{
namespace argparser
{
class Flag
{
public:
    Flag() = default;
    virtual ~Flag() = default;
    virtual bool match(const std::string &) = 0;
    virtual std::string to_string() const = 0;
    virtual bool apply(const std::string &value) = 0;
    virtual bool apply_default() = 0;
    virtual std::string short_name() const = 0;
    virtual std::string full_name() const = 0;
    virtual std::string desc() const = 0;
};
template <typename T>
class ConcreteFlag : public Flag
{
public:
    ConcreteFlag(T *flag,
                 const std::string &full_name,
                 const std::string &short_name,
                 const std::string &desc,
                 const std::optional<std::string> &default_val)
        : flag_(flag),
          full_name_(full_name),
          short_name_(short_name),
          desc_(desc),
          default_(default_val)
    {
    }
    static std::shared_ptr<ConcreteFlag<T>> make_flag(
        T *flag,
        const std::string &full_name,
        const std::string &short_name,
        const std::string &desc,
        const std::optional<std::string> &default_val = std::nullopt)
    {
        return std::make_shared<ConcreteFlag<T>>(
            flag, full_name, short_name, desc, default_val);
    }
    bool match(const std::string &key) override
    {
        if (key.empty())
        {
            return false;
        }
        return key == full_name_ || key == short_name_;
    }
    std::string to_string() const override
    {
        return "{Flag " + full_name_ + ", " + short_name_ + "}";
    }
    bool apply(const std::string &value) override;
    bool apply_default() override
    {
        if (default_.has_value())
        {
            return apply(default_.value());
        }
        return false;
    }
    std::string short_name() const override
    {
        return short_name_;
    }
    std::string full_name() const override
    {
        return full_name_;
    }
    std::string desc() const override
    {
        return desc_;
    }
    ~ConcreteFlag() = default;

private:
    T *flag_;
    std::string full_name_;
    std::string short_name_;
    std::string desc_;
    std::optional<std::string> default_;
};
template <typename T>
bool ConcreteFlag<T>::apply(const std::string &value)
{
    std::istringstream iss(value);
    iss >> *flag_;
    return !iss.fail();
}
template <>
bool ConcreteFlag<bool>::apply(const std::string &value)
{
    if (value.empty() || value == "1" || value == "true")
    {
        *flag_ = true;
        return true;
    }
    else if (value == "0" || value == "false")
    {
        *flag_ = false;
        return true;
    }
    return false;
}

class Parser
{
public:
    Parser(const std::string &description) : description_(description)
    {
    }
    void print_promt() const
    {
        std::cout << description_ << std::endl << std::endl;

        print_usage();
        print_flags();
    }
    void print_usage() const
    {
        if (!flags_.empty())
        {
            std::cout << "Usage:" << std::endl
                      << "  " << program_name << " [flag]" << std::endl
                      << std::endl;
        }
    }
    void print_flags() const
    {
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
    template <typename T>
    bool flag(T *flag,
              const std::string &full_name,
              const std::string &short_name,
              const std::string &desc,
              const std::optional<std::string> &default_val)
    {
        return reg_flag(flag, full_name, short_name, desc, default_val, false);
    }
    template <typename T>
    bool flag(T *flag,
              const std::string &full_name,
              const std::string &short_name,
              const std::string &desc)
    {
        return reg_flag(flag, full_name, short_name, desc, std::nullopt, true);
    }
    template <typename T>
    bool reg_flag(T *flag,
                  const std::string &full_name,
                  const std::string &short_name,
                  const std::string &desc,
                  const std::optional<std::string> &default_val,
                  bool required)
    {
        if (!validate_full_flag(full_name))
        {
            std::cerr << "Failed to register flag " << full_name
                      << ": identity not allowed" << std::endl;
            return false;
        }
        if (!validate_short_flag(short_name))
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
        flags_.emplace_back(ConcreteFlag<T>::make_flag(
            flag, full_name, short_name, desc, default_val));
        required_.emplace_back(required);
        parsed_.emplace_back(false);
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
    using KVPair = std::tuple<std::string, std::string>;
    using KVPairs = std::vector<KVPair>;
    bool is_flag(const std::string &str)
    {
        return !str.empty() && str[0] == '-';
    }
    bool validate_full_flag(const std::string &name) const
    {
        return name.size() >= 3 && name[0] == '-' && name[1] == '-' &&
               isalpha(name[2]);
    }
    bool validate_short_flag(const std::string &name) const
    {
        return name.empty() ||
               (name.size() >= 2 && name[0] == '-' && isalpha(name[1]));
    }
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
    KVPairs retrieve(int argc, char *argv[])
    {
        KVPairs ret;
        // skip program name, i start from 1
        for (int i = 1; i < argc; ++i)
        {
            std::string command_opt(argv[i]);
            /**
             * It is a command.
             * e.g. ./program test
             */
            if (!is_flag(command_opt))
            {
                ret.emplace_back(std::move(command_opt), "");
                continue;
            }

            /**
             * separate:
             * --time 5
             * not separate:
             * --time=5
             */
            std::string key;
            std::string value;
            auto equal_pos = command_opt.find('=');
            bool separate = equal_pos == std::string::npos;
            if (!separate)
            {
                key = command_opt.substr(0, equal_pos);
                value = command_opt.substr(equal_pos + 1);
            }
            else
            {
                /**
                 * value may be the next argv
                 * e.g. --time 5
                 *
                 * or it's a flag with no value
                 * e.g. --flag_on
                 */
                key = command_opt;
                if (i + 1 >= argc || argv[i + 1][0] == '-')
                {
                    // no value.
                }
                else
                {
                    // value is the next argv
                    value = argv[i + 1];
                    // skip the next argv
                    i++;
                }
            }
            ret.emplace_back(key, value);
        }
        return ret;
    }
    bool apply(int argc, char *argv[])
    {
        program_name = argv[0];

        auto pairs = retrieve(argc, argv);
        for (const auto &[key, value] : pairs)
        {
            if (!is_flag(key))
            {
                std::cerr << "TODO: skip key " << key << std::endl;
                continue;
            }
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
                    parsed_[i] = true;
                }
            }
        }
        for (size_t i = 0; i < parsed_.size(); ++i)
        {
            if (!parsed_[i])
            {
                flags_[i]->apply_default();
            }
        }

        // check whether all the required flags are provided
        for (size_t i = 0; i < required_.size(); ++i)
        {
            if (required_[i] && !parsed_[i])
            {
                std::cerr << flags_[i]->to_string() << " not provided."
                          << std::endl;
                return false;
            }
        }
        return true;
    }

private:
    std::string description_;
    std::set<std::string> registered_full_flags_;
    std::set<std::string> registered_short_flags_;
    std::vector<std::shared_ptr<Flag>> flags_;
    std::vector<bool> required_;
    std::vector<bool> parsed_;
    std::string program_name;
    size_t max_full_name_len_{0};
    size_t max_short_name_len_{0};
};  // namespace argparser
static Parser &init(const std::string &description = {})
{
    static std::shared_ptr<Parser> parser;
    if (parser == nullptr)
    {
        parser = std::make_shared<Parser>(description);
    }
    return *parser;
}
}  // namespace argparser
}  // namespace hello
#endif
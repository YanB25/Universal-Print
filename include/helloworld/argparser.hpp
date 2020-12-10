#ifndef ARG_PARSER_H
#define ARG_PARSER_H
#include <memory>
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
};
template <typename T>
class ConcreteFlag : public Flag
{
public:
    ConcreteFlag(T *flag,
                 const std::string &full_name,
                 const std::string &short_name,
                 const std::string &desc,
                 const std::string &default_val)
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
        const std::string &default_val = {})
    {
        return std::make_shared<ConcreteFlag<T>>(
            flag, full_name, short_name, desc, default_val);
    }
    bool match(const std::string &key) override
    {
        return key == full_name_ || key == short_name_;
    }
    std::string to_string() const override
    {
        return "{Flag " + full_name_ + ", " + short_name_ + "}";
    }
    bool apply(const std::string &value) override;
    bool apply_default() override
    {
        return apply(default_);
    }
    ~ConcreteFlag() = default;

private:
    T *flag_;
    std::string full_name_;
    std::string short_name_;
    std::string desc_;
    std::string default_;
};
template <typename T>
bool ConcreteFlag<T>::apply(const std::string &value)
{
    std::istringstream iss(value);
    iss >> *flag_;
    return !iss.fail();
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
    }
    template <typename T>
    void flag(T *flag,
              const std::string &full_name,
              const std::string &short_name,
              const std::string &desc,
              const std::string &default_val)
    {
        flags_.emplace_back(ConcreteFlag<T>::make_flag(
            flag, full_name, short_name, desc, default_val));
        required_.emplace_back(false);
        parsed_.emplace_back(false);
    }
    template <typename T>
    void flag(T *flag,
              const std::string &full_name,
              const std::string &short_name,
              const std::string &desc)
    {
        flags_.emplace_back(
            ConcreteFlag<T>::make_flag(flag, full_name, short_name, desc, T{}));
        required_.emplace_back(true);
        parsed_.emplace_back(false);
    }
    using KVPair = std::tuple<std::string, std::string>;
    using KVPairs = std::vector<KVPair>;
    bool is_flag(const std::string &str)
    {
        return !str.empty() && str[0] == '-';
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
    std::vector<std::shared_ptr<Flag>> flags_;
    std::vector<bool> required_;
    std::vector<bool> parsed_;
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
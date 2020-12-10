#ifndef ARG_PARSER_H
#define ARG_PARSER_H
#include <cctype>
#include <iostream>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "./common.hpp"
#include "./flag-manager.hpp"
namespace hello
{
namespace argparser
{
class Parser
{
public:
    Parser(const std::string &description)
        : description_(description),
          flag_manager_(flag::FlagManager::new_instance())
    {
    }
    void print_promt() const
    {
        if (!init_)
        {
            std::cerr << "ERR: parse() not called." << std::endl;
            return;
        }
        std::cout << description_ << std::endl << std::endl;

        print_usage();
        flag_manager_->print_flags();
    }
    template <typename T>
    bool flag(T *flag,
              const std::string &full_name,
              const std::string &short_name,
              const std::string &desc,
              const std::optional<std::string> &default_val)
    {
        return flag_manager_->add_flag(
            flag, full_name, short_name, desc, default_val, false);
    }
    template <typename T>
    bool flag(T *flag,
              const std::string &full_name,
              const std::string &short_name,
              const std::string &desc)
    {
        return flag_manager_->add_flag(
            flag, full_name, short_name, desc, std::nullopt, true);
    }
    bool parse(int argc, char *argv[])
    {
        init_ = true;
        program_name = argv[0];

        auto pairs = retrieve(argc, argv);
        for (const auto &[key, value] : pairs)
        {
            if (!flag::is_flag(key))
            {
                std::cerr << "TODO: skip key " << key << std::endl;
                continue;
            }
            if (!flag_manager_->apply(key, value))
            {
                return false;
            }
        }

        auto missing_keys = flag_manager_->missing_keys();
        if (!missing_keys.empty())
        {
            std::cerr << "Failed to parse command line: [";
            for (const auto &[full_name, short_name] : missing_keys)
            {
                std::cerr << "{Flag " << full_name << ", " << short_name
                          << "}, ";
            }
            std::cerr << "] are required but not provided." << std::endl;
            return false;
        }
        return true;
    }

private:
    bool init_{false};
    std::string program_name;
    std::string description_;

    flag::FlagManager::Pointer flag_manager_;

    void print_usage() const
    {
        if (!flag_manager_->empty())
        {
            std::cout << "Usage:" << std::endl
                      << "  " << program_name << " [flag]" << std::endl
                      << std::endl;
        }
    }
    using KVPair = std::tuple<std::string, std::string>;
    using KVPairs = std::vector<KVPair>;
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
            if (!flag::is_flag(command_opt))
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
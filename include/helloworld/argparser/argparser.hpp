#ifndef ARG_PARSER_H
#define ARG_PARSER_H
#include <cctype>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "./common.hpp"
#include "./debug.hpp"
#include "./flag-manager.hpp"
namespace argparser
{
// TODO: unable to know the current command
class Parser
{
public:
    using Pointer = std::unique_ptr<Parser>;
    using FlagPair = std::pair<std::string, std::string>;
    using FlagPairs = std::list<FlagPair>;
    Parser(std::shared_ptr<flag::FlagManager> global_flag_manager,
           const std::string &description)
        : description_(description),
          flag_manager_(flag::FlagManager::new_instance()),
          gf_manager_(global_flag_manager)
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
        print_command();
        flag_manager_->print_flags();
        gf_manager_->print_flags("Global Flag");
    }
    void print_promt(FlagPairs &pairs) const
    {
        for (auto it = pairs.begin(); it != pairs.end(); ++it)
        {
            const auto &key = it->first;
            if (flag::is_flag(key))
            {
                if (flag_manager_->contain(key))
                {
                    // this flag is expected, we can keep going.
                    continue;
                }
                else
                {
                    // this flag is not expected, stop here.
                    break;
                }
            }
            else
            {
                // this is a command
                auto parser_it = sub_parsers_.find(key);
                if (parser_it == sub_parsers_.end())
                {
                    break;
                }
                pairs.erase(pairs.begin(), ++it);
                return parser_it->second->print_promt(pairs);
            }
        }
        print_promt();
    }
    void print_promt(int argc, const char *argv[]) const
    {
        auto pairs = retrieve(argc, argv);
        return print_promt(pairs);
    }
    std::string desc() const
    {
        return description_;
    }
    Parser &command(const std::string &command, const std::string &desc = {})
    {
        sub_parsers_.emplace(command,
                             std::make_unique<Parser>(gf_manager_, desc));
        max_command_len_ = std::max(max_command_len_, command.size());
        return *sub_parsers_[command];
    }
    // TODO: make default has type?
    template <typename T>
    bool flag(T *flag,
              const std::string &full_name,
              const std::string &short_name,
              const std::string &desc,
              const std::optional<std::string> &default_val)
    {
        if (gf_manager_->contain(full_name) || gf_manager_->contain(short_name))
        {
            std::cerr << "Flag registered failed: flag \"" << full_name
                      << "\", \"" << short_name
                      << "\" conflict with global flag" << std::endl;
            return false;
        }
        return flag_manager_->add_flag(
            flag, full_name, short_name, desc, default_val, false);
    }
    template <typename T>
    bool flag(T *flag,
              const std::string &full_name,
              const std::string &short_name,
              const std::string &desc)
    {
        if (gf_manager_->contain(full_name) || gf_manager_->contain(short_name))
        {
            std::cerr << "Flag registered failed: flag \"" << full_name
                      << "\", \"" << short_name
                      << "\" conflict with global flag" << std::endl;
            return false;
        }
        return flag_manager_->add_flag(
            flag, full_name, short_name, desc, std::nullopt, true);
    }
    template <typename T>
    bool global_flag(T *flag,
                     const std::string &full_name,
                     const std::string &short_name,
                     const std::string &desc)
    {
        if (flag_manager_->contain(full_name) ||
            flag_manager_->contain(short_name))
        {
            std::cerr << "Flag registered failed: flag \"" << full_name
                      << "\", \"" << short_name << "\" conflict detected."
                      << std::endl;
            return false;
        }
        return gf_manager_->add_flag(
            flag, full_name, short_name, desc, std::nullopt, true);
    }
    template <typename T>
    bool global_flag(T *flag,
                     const std::string &full_name,
                     const std::string &short_name,
                     const std::string &desc,
                     const std::optional<std::string> &default_val)
    {
        if (flag_manager_->contain(full_name) ||
            flag_manager_->contain(short_name))
        {
            std::cerr << "Flag registered failed: flag \"" << full_name
                      << "\", \"" << short_name << "\" conflict detected."
                      << std::endl;
            return false;
        }
        return gf_manager_->add_flag(
            flag, full_name, short_name, desc, default_val, false);
    }
    bool parse(int argc, const char *argv[])
    {
        command_path_.clear();
        program_name = argv[0];

        auto pairs = retrieve(argc, argv);
        return do_parse(pairs, command_path_);
    }
    std::vector<std::string> command_path() const
    {
        return command_path_;
    }

private:
    bool init_{false};
    std::string program_name;
    std::string description_;

    flag::FlagManager::Pointer flag_manager_;
    flag::FlagManager::Pointer gf_manager_;
    std::unordered_map<std::string, Pointer> sub_parsers_;
    size_t max_command_len_{0};

    std::vector<std::string> command_path_;

    void print_usage() const
    {
        if (!flag_manager_->empty() || !sub_parsers_.empty())
        {
            std::cout << "Usage:" << std::endl;
        }
        if (!sub_parsers_.empty())
        {
            std::cout << std::string(8, ' ') << program_name << " [command]"
                      << std::endl;
        }
        if (!flag_manager_->empty())
            std::cout << std::string(8, ' ') << program_name << " [flag]"
                      << std::endl;
        if (!flag_manager_->empty() || !sub_parsers_.empty())
        {
            std::cout << std::endl;
        }
    }
    const flag::FlagManager::Pointer flag_manager() const
    {
        return flag_manager_;
    }
    void print_command() const
    {
        if (!sub_parsers_.empty())
        {
            std::cout << "Available Commands:" << std::endl;
            for (const auto &[command, parser] : sub_parsers_)
            {
                std::cout << "  " << command;
                std::cout << std::string(max_command_len_ + 2, ' ');
                std::cout << parser->desc() << std::endl;
            }
            std::cout << std::endl;
        }
    }
    bool do_parse(FlagPairs &pairs, std::vector<std::string>& command_path)
    {
        init_ = true;

        for (auto pair_it = pairs.begin(); pair_it != pairs.end(); ++pair_it)
        {
            const auto &key = pair_it->first;
            const auto &value = pair_it->second;

            /**
             * It is either a flag or a command.
             * A flag: ./program --test
             * A command: ./program start
             *
             * If it is a command, we delegate the parsing to the sub_parser
             */
            if (!flag::is_flag(key))
            {
                auto parser_it = sub_parsers_.find(key);
                if (parser_it == sub_parsers_.end())
                {
                    std::cerr << "Failed to parse command \"" << key
                              << "\": use --help for usage." << std::endl;
                    return false;
                }
                auto sub_parser = parser_it->second.get();
                // remove [begin, pair_it] and pass the remaining to the
                // sub_parser
                pairs.erase(pairs.begin(), ++pair_it);
                command_path.push_back(key);
                return sub_parser->do_parse(pairs, command_path);
            }

            if (!flag_manager_->apply(key, value) &&
                !gf_manager_->apply(key, value))
            {
                std::cerr << "Failed to apply " << key << "=\"" << value
                          << "\": Failure due to previous problem" << std::endl;
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
    static FlagPairs retrieve(int argc, const char *argv[])
    {
        FlagPairs ret;
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
                if (i + 1 >= argc || flag::is_flag(argv[i + 1]))
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
std::shared_ptr<Parser> new_parser(const std::string &desc = {})
{
    auto global_flag_manager = std::make_shared<flag::FlagManager>();
    return std::make_shared<Parser>(global_flag_manager, desc);
}

Parser &init(const std::string &desc)
{
    static std::shared_ptr<Parser> root_parser;
    if (root_parser == nullptr)
    {
        root_parser = new_parser(desc);
    }
    return *root_parser;
}
namespace impl
{
std::shared_ptr<Parser> new_parser(
    std::shared_ptr<flag::FlagManager> global_flag_manager,
    const std::string &desc = {})
{
    return std::make_shared<Parser>(global_flag_manager, desc);
}

}  // namespace impl
}  // namespace argparser
#endif
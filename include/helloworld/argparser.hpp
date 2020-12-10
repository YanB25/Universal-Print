#ifndef ARG_PARSER_H
#define ARG_PARSER_H
#include <memory>
#include <vector>
#include <unordered_map>
namespace hello
{
namespace argparser
{
class Slot
{
public:
    virtual ~Slot() = default;
};
template <typename T>
class ConcreteSlot : public Slot
{
public:
    ConcreteSlot(T* slot): slot_(slot)
    {
    }
    static ConcreteSlot<T> make_slot(T* slot)
    {
        return std::make_shared<ConcreteSlot<T>>(slot);
    }
private:
    T* slot_;
};
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
    void flag(const std::string &full_name,
              const std::string &short_name,
              const std::string &desc,
              T *slot,
              const T& default_val = {})
    {
        full_flags.emplace(full_name, ConcreteSlot<T>::make_slot(slot));
    }

private:
    std::string description_;
    std::unordered_map<std::string, std::shared_ptr<Slot>> full_flags;
};
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
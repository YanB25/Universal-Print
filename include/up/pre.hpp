#pragma once
#ifndef UNIVERSAL_PRESENTATION_H_
#define UNIVERSAL_PRESENTATION_H_

#include <array>
#include <atomic>
#include <deque>
#include <forward_list>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Implementation Details
// We maintain use two type:
// - util::pre
// - util::pre_impl
// * util::pre is the black-box interface
// Any cout of pre<T> will be trapped into cout of pre_impl<T>, with ctx.depth
// minused by one automatically.
// If pre_impl<T> is not cout-able, it fallback to cout T
// T, itself, must be cout-able. Otherwise, COMPILE ERROR

// Reasoning:
// The trap from util::pre<T> to util::impl_pre<T>
// give a chance of automatically minus ctx.depth by one.

// For any T that steps in one level logically
// e.g., std::vector<T>, std::deque<T>
// let util::pre_impl<T> be cout-able, so that ctx.depth is updated

// For any helper type T, i.e., those behave transparently
// e.g., std::queue<T> is helper of std::deque<T>
// e.g., std_container_present_impl is a helper of T
// let util::pre<T> be cout-able, so that ctx.depth is not touched

namespace util
{
template <typename T, typename = void>
auto constexpr ostreamable_v = false;
template <typename T>
auto constexpr ostreamable_v<
    T,
    std::void_t<decltype(std::cout << std::declval<T>())>> = true;

namespace names
{
// Checks magic_enum compiler compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || \
    defined(__GNUC__) && __GNUC__ >= 9 ||         \
    defined(_MSC_VER) && _MSC_VER >= 1910
#undef MAGIC_ENUM_SUPPORTED
#define MAGIC_ENUM_SUPPORTED 1
#endif

#ifdef MAGIC_ENUM_SUPPORTED
std::string_view pretty_name(std::string_view name) noexcept
{
    for (std::size_t i = name.size(); i > 0; --i)
    {
        if (name[i - 1] == '=')
        {
            name.remove_prefix(i);
        }
    }
    if (name.size() > 0 && name[0] == ' ')
    {
        name.remove_prefix(1);
        return name;
    }
    return {};  // Invalid name.
}

template <typename E, E V>
constexpr auto get_type_value_name() noexcept
{
    static_assert(std::is_enum_v<E>,
                  "magic_enum::detail::n requires enum type.");
    auto name =
        pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
    return name;
}

template <typename E>
constexpr auto get_type_name() noexcept
{
    auto name =
        pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
    return name;
}
#else

template <typename E>
constexpr auto get_type_name() noexcept
{
    return std::string_view{};
}

template <typename E, E V>
constexpr auto get_type_value_name() noexcept
{
    return std::string_view{};
}

#endif
}  // namespace names

struct pre_ctx
{
    pre_ctx(ssize_t l, ssize_t d, bool h) : limit(l), depth(d), human(h)
    {
    }
    pre_ctx() = default;
    ssize_t limit{std::numeric_limits<decltype(limit)>::max()};
    ssize_t depth{std::numeric_limits<decltype(depth)>::max()};
    bool human{false};
};

template <typename T>
class pre_impl
{
public:
    static constexpr ssize_t kMax =
        std::numeric_limits<decltype(pre_ctx::limit)>::max();
    pre_impl(const T &t,
             ssize_t limit = kMax,
             ssize_t depth = kMax,
             bool human = false)
        : t_(t), ctx_(pre_ctx{limit, depth, human})
    {
    }
    pre_impl(const T &t, const pre_ctx &ctx) : t_(t), ctx_(ctx)
    {
    }
    ssize_t limit() const
    {
        return ctx_.limit;
    }
    bool human() const
    {
        return ctx_.human;
    }
    const T &inner() const
    {
        return t_;
    }
    const pre_ctx &ctx() const
    {
        return ctx_;
    }
    pre_ctx next_ctx() const
    {
        auto ret = ctx_;
        ret.depth--;
        return ret;
    }

private:
    const T &t_;
    pre_ctx ctx_;
};

// Fallback cout of pre_impl<T> to cout of T itself.
template <typename T, std::enable_if_t<ostreamable_v<T>, bool> = true>
inline std::ostream &operator<<(std::ostream &os, const pre_impl<T> &t)
{
    os << t.inner();
    return os;
}

template <typename T>
class pre
{
public:
    static constexpr ssize_t kMax =
        std::numeric_limits<decltype(pre_ctx::limit)>::max();
    pre(const T &t,
        ssize_t limit = kMax,
        ssize_t depth = kMax,
        bool human = false)
        : t_(t), ctx_(pre_ctx{limit, depth, human})
    {
    }
    pre(const T &t, const pre_ctx &ctx) : t_(t), ctx_(ctx)
    {
    }
    ssize_t limit() const
    {
        return ctx_.limit;
    }
    bool human() const
    {
        return ctx_.human;
    }
    const T &inner() const
    {
        return t_;
    }
    const pre_ctx &ctx() const
    {
        return ctx_;
    }
    pre_ctx next_ctx() const
    {
        auto ret = ctx_;
        ret.depth--;
        return ret;
    }

private:
    const T &t_;
    pre_ctx ctx_;
};

// Fallback cout of pre<T> to pre_impl<T>,
// with ctx.depth updated.
// Stop going further if ctx.depth <= 0
template <typename T>
inline std::ostream &operator<<(std::ostream &os, const pre<T> &t)
{
    auto ctx = t.ctx();
    ctx.depth--;
    if (ctx.depth < 0)
    {
        os << "...";
        return os;
    }
    else
    {
        // ctx.depth >= 0
        os << pre_impl<T>(t.inner(), ctx);
    }
    return os;
}

template <typename T, typename = void>
auto constexpr universally_presentable_v = false;
template <typename T>
auto constexpr universally_presentable_v<
    T,
    std::void_t<decltype(std::cout << util::pre_impl(std::declval<T>()))>> =
    true;

// below are helper class to combine the formating logics
template <typename T>
struct std_container_present_impl
{
    std_container_present_impl(const T &t,
                               char lb = '[',
                               char rb = ']',
                               char sep = ',')
        : t_(t), lb_(lb), rb_(rb), sep_(sep)
    {
    }
    const T &inner() const
    {
        return t_;
    }
    char lb() const
    {
        return lb_;
    }
    char rb() const
    {
        return rb_;
    }
    char sep() const
    {
        return sep_;
    }
    const T &t_;
    char lb_;
    char rb_;
    char sep_;
};

// overloading pre<T>, does not touch ctx
template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre<std_container_present_impl<T>> t)
{
    const auto &pre = t.inner();
    const auto &obj = pre.inner();
    auto ctx = t.ctx();  // copy here

    size_t start_size = 0;
    size_t end_size = 0;
    size_t ommitted = 0;
    size_t output_size = 0;
    ssize_t obj_size = std::size(obj);
    bool start_has_break = false;
    if (t.limit() >= obj_size)
    {
        start_size = obj_size;
        end_size = 0;
        output_size = obj_size;
        ommitted = 0;
    }
    else
    {
        start_size = (t.limit() + 1) / 2;
        end_size = t.limit() - start_size;
        output_size = start_size + end_size;
        ommitted = obj_size - output_size;
    }

    size_t outputed_nr = 0;

    os << pre.lb();
    auto front_iter = std::cbegin(obj);
    for (size_t i = 0; i < start_size; ++i)
    {
        os << util::pre(*front_iter, ctx);
        outputed_nr++;
        if (outputed_nr < output_size)
        {
            os << pre.sep() << " ";
            start_has_break = true;
        }
        front_iter++;
    }
    if (ommitted)
    {
        if (start_size && !start_has_break)
        {
            os << pre.sep() << " ";
        }
        os << "...";

        if (end_size > 0)
        {
            os << pre.sep() << " ";
        }
    }
    // auto back_iter = obj.rbegin();
    auto back_iter = std::rbegin(obj);

    std::advance(back_iter, end_size);
    for (size_t i = 0; i < end_size; ++i)
    {
        bool last = i + 1 == end_size;
        if (!last)
        {
            os << util::pre(*back_iter, ctx) << pre.sep() << " ";
        }
        else
        {
            os << util::pre(*back_iter, ctx);
        }
        back_iter--;
    }
    os << pre.rb();
    if (t.human())
    {
        os << " (sz: " << obj_size << ", ommitted " << ommitted << ")";
    }
    return os;
}

template <typename T>
struct std_forward_container_present_impl
{
    std_forward_container_present_impl(const T &t,
                                       char lb = '[',
                                       char rb = ']',
                                       char sep = ',')
        : t_(t), lb_(lb), rb_(rb), sep_(sep)
    {
    }
    const T &inner() const
    {
        return t_;
    }
    char lb() const
    {
        return lb_;
    }
    char rb() const
    {
        return rb_;
    }
    char sep() const
    {
        return sep_;
    }
    const T &t_;
    char lb_;
    char rb_;
    char sep_;
};

template <typename T>
inline std::ostream &operator<<(
    std::ostream &os, const pre<std_forward_container_present_impl<T>> t)
{
    const auto &pre = t.inner();
    const auto &obj = pre.inner();
    auto ctx = t.ctx();  // must copy here

    size_t output_size = t.limit();

    os << pre.lb();
    auto front_iter = obj.cbegin();
    bool has_more = true;

    if (front_iter == obj.cend())
    {
        has_more = false;
    }
    else
    {
        for (size_t i = 0; i < output_size; ++i)
        {
            os << util::pre(*front_iter, ctx);
            front_iter++;
            if (front_iter == obj.cend())
            {
                has_more = false;
                break;
            }
            os << ", ";
        }
    }
    if (has_more)
    {
        os << "...";
    }
    os << pre.rb();
    return os;
}

// Magic to print std::tuple<Types...>
template <typename TupType, size_t... I>
struct std_tuple_present_impl
{
    std_tuple_present_impl(const TupType &t, std::index_sequence<I...> i)
        : t_(t), i_(i)
    {
    }
    const TupType &t_;
    const std::index_sequence<I...> i_;
};

template <typename TupType, size_t... I>
inline std::ostream &operator<<(std::ostream &os,
                                std_tuple_present_impl<TupType, I...> &t)
{
    os << "<";
    (..., (os << (I == 0 ? "" : ", ") << util::pre(std::get<I>(t.t_))));
    os << ">";
    return os;
}

template <typename... Types>
inline std::ostream &operator<<(std::ostream &os,
                                const pre<std::tuple<Types...>> t)
{
    auto tup = util::std_tuple_present_impl(
        t.inner(), std::make_index_sequence<sizeof...(Types)>());
    os << tup;
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre_impl<std::vector<T>> &vec)
{
    os << util::pre(std_container_present_impl(vec.inner(), '[', ']', ','),
                    vec.ctx());
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre_impl<std::list<T>> &lst)
{
    os << util::pre(std_container_present_impl(lst.inner(), '[', ']', ','),
                    lst.ctx());
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre_impl<std::forward_list<T>> &lst)
{
    os << util::pre(
        std_forward_container_present_impl(lst.inner(), '[', ']', ','),
        lst.ctx());
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre_impl<std::set<T>> &set)
{
    os << util::pre(std_container_present_impl(set.inner(), '{', '}', ','),
                    set.ctx());
    return os;
}
template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre_impl<std::multiset<T>> &set)
{
    os << util::pre(std_container_present_impl(set.inner(), '{', '}', ','),
                    set.ctx());
    return os;
}
template <typename K, typename V>
inline std::ostream &operator<<(std::ostream &os,
                                const pre_impl<std::map<K, V>> &m)
{
    os << util::pre(std_container_present_impl(m.inner(), '{', '}', ','),
                    m.ctx());
    return os;
}
template <typename K, typename V>
inline std::ostream &operator<<(std::ostream &os,
                                const pre_impl<std::multimap<K, V>> &m)
{
    os << util::pre(std_container_present_impl(m.inner(), '{', '}', ','),
                    m.ctx());
    return os;
}
template <typename K, typename V>
inline std::ostream &operator<<(std::ostream &os,
                                const pre_impl<std::unordered_map<K, V>> &m)
{
    os << util::pre(
        std_forward_container_present_impl(m.inner(), '{', '}', ','), m.ctx());
    return os;
}
template <typename K, typename V>
inline std::ostream &operator<<(
    std::ostream &os, const pre_impl<std::unordered_multimap<K, V>> &m)
{
    os << util::pre(
        std_forward_container_present_impl(m.inner(), '{', '}', ','), m.ctx());
    return os;
}
template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre_impl<std::unordered_set<T>> &set)
{
    os << util::pre(
        std_forward_container_present_impl(set.inner(), '{', '}', ','),
        set.ctx());
    return os;
}
template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre_impl<std::unordered_multiset<T>> &set)
{
    os << util::pre(
        std_forward_container_present_impl(set.inner(), '{', '}', ','),
        set.ctx());
    return os;
}
template <typename T, size_t size>
inline std::ostream &operator<<(std::ostream &os,
                                const pre_impl<std::array<T, size>> &p)
{
    os << util::pre(std_container_present_impl(p.inner(), '[', ']', ','),
                    p.ctx());
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre_impl<std::deque<T>> &p)
{
    os << util::pre(std_container_present_impl(p.inner(), '[', ']', ','),
                    p.ctx());
    return os;
}

template <typename T,
          size_t size,
          std::enable_if_t<!std::is_same_v<T, char>, bool> = true>
using c_style_array = T[size];

template <typename T, size_t size>
inline std::ostream &operator<<(std::ostream &os,
                                const pre_impl<c_style_array<T, size>> &p)
{
    std_container_present_impl c_array_pre(p.inner());
    auto a = util::pre(c_array_pre, p.ctx());
    os << a;
    return os;
}

template <size_t size>
using c_style_string = char[size];

template <size_t size>
inline std::ostream &operator<<(std::ostream &os,
                                const pre<c_style_string<size>> &p)
{
    os << "\"" << (const char *) p.inner() << "\"";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const pre<std::string> &p)
{
    os << "\"" << p.inner() << "\"";
    return os;
}
inline std::ostream &operator<<(std::ostream &os, const pre<std::byte> &p)
{
    os << "'" << (char) p.inner() << "'";
    return os;
}
inline std::ostream &operator<<(std::ostream &os, const pre<char> &p)
{
    os << "'" << p.inner() << "'";
    return os;
}

// We do not consider std::pair, std::tuple, std::optinoal, std::atomic, ...
// as one step deeper.
// So, use pre<> instead of pre_impl<>
template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre<std::optional<T>> &t)
{
    if (t.inner().has_value())
    {
        os << "some(" << util::pre(t.inner().value(), t.ctx()) << ")";
    }
    else
    {
        os << "nullopt";
    }
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const pre<std::nullopt_t> &)
{
    os << "nullopt";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const pre<bool> &p)
{
    if (p.inner())
    {
        os << "true";
    }
    else
    {
        os << "false";
    }
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const pre<std::atomic<T>> &t)
{
    os << "atomic("
       << util::pre(t.inner().load(std::memory_order_relaxed), t.ctx()) << ")";
    return os;
}

template <typename T, typename U>
inline std::ostream &operator<<(std::ostream &os, const pre<std::pair<T, U>> &s)
{
    const auto &t = s.inner();
    os << "(" << util::pre(t.first, s.ctx()) << ", "
       << util::pre(t.second, s.ctx()) << ")";
    return os;
}

// a hack to get the internal container of any adaptors
// https://stackoverflow.com/questions/1185252/is-there-a-way-to-access-the-underlying-container-of-stl-container-adaptors
template <class ADAPTER>
const typename ADAPTER::container_type &get_container(const ADAPTER &a)
{
    struct hack : ADAPTER
    {
        static const typename ADAPTER::container_type &get(const ADAPTER &a)
        {
            return a.*(&hack::c);
        }
    };
    return hack::get(a);
}

// for adaptors, we fordward straightly.
// Use pre<> instead of pre_impl<>
template <typename T>
inline std::ostream &operator<<(std::ostream &os, const pre<std::queue<T>> &s)
{
    const auto &t = s.inner();
    const auto &c = get_container(t);
    os << util::pre(c, s.ctx());
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const pre<std::stack<T>> &s)
{
    const auto &t = s.inner();
    const auto &c = get_container(t);
    os << util::pre(c, s.ctx());
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre<std::priority_queue<T>> &s)
{
    const auto &t = s.inner();
    const auto &c = get_container(t);
    os << util::pre(c, s.ctx());
    return os;
}

template <typename T>
std::string pre_str(const T &t)
{
    std::stringstream ss;
    ss << util::pre(t);
    return ss.str();
}

// https://github.com/Sinacam/ezprint
// https://stackoverflow.com/questions/64777768/easy-way-to-cout-any-struct-in-c
namespace fallback
{
struct ubiq
{
    template <typename T>
    operator T();
};

template <size_t>
using ubiq_t = ubiq;

// select the one with T{Ubiqs{}...} a valid statement (compiled and has type)
// Since Ubiqs{} can turn into any types,
// It only succeeds when number of Ubiqs{}... equals to the # of fields of T.
// With too many Ubiqs{}..., CE: excess elements in struct initializer
template <typename T, typename... Ubiqs>
constexpr auto count_r(size_t &sz, int) -> std::void_t<decltype(T{Ubiqs{}...})>
{
    sz = sizeof...(Ubiqs);
}

// effectively calling count_r<T, Ubiqs - 1>
template <typename T, typename, typename... Ubiqs>
constexpr auto count_r(size_t &sz, float)
{
    count_r<T, Ubiqs...>(sz, 0);
}

// Return the number of fields in T
template <typename T, size_t... Is>
constexpr auto count(std::index_sequence<Is...>)
{
    size_t sz = 0;
    count_r<T, ubiq_t<Is>...>(sz, 0);
    return sz;
}

constexpr static size_t kMaxFieldNr = 32;

template <typename T>
constexpr auto count()
{
    constexpr size_t count_nr = std::min(sizeof(T), kMaxFieldNr);
    return count<T>(std::make_index_sequence<count_nr>{});
}
template <typename T>
inline auto as_tuple(T &&, std::integral_constant<size_t, 0>)
{
    return std::tuple{};
}

#define DEFINE_AS_TUPLE(N, ...)                                    \
    template <typename T>                                          \
    inline auto as_tuple(T &&t, std::integral_constant<size_t, N>) \
    {                                                              \
        static_assert(N <= kMaxFieldNr, "Tune up kMaxFieldNr");    \
        auto &[__VA_ARGS__] = t;                                   \
        return std::forward_as_tuple(__VA_ARGS__);                 \
    }
// clang-format off
DEFINE_AS_TUPLE(1, x0)
DEFINE_AS_TUPLE(2, x0, x1)
DEFINE_AS_TUPLE(3, x0, x1, x2)
DEFINE_AS_TUPLE(4, x0, x1, x2, x3)
DEFINE_AS_TUPLE(5, x0, x1, x2, x3, x4)
DEFINE_AS_TUPLE(6, x0, x1, x2, x3, x4, x5)
DEFINE_AS_TUPLE(7, x0, x1, x2, x3, x4, x5, x6)
DEFINE_AS_TUPLE(8, x0, x1, x2, x3, x4, x5, x6, x7)
DEFINE_AS_TUPLE(9, x0, x1, x2, x3, x4, x5, x6, x7, x8)
DEFINE_AS_TUPLE(10, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9)
DEFINE_AS_TUPLE(11, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10)
DEFINE_AS_TUPLE(12, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11)
DEFINE_AS_TUPLE(13, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12)
DEFINE_AS_TUPLE(14, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13)
DEFINE_AS_TUPLE(15, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14)
DEFINE_AS_TUPLE(16, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15)
DEFINE_AS_TUPLE(17, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16)
DEFINE_AS_TUPLE(18, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,
                x17)
DEFINE_AS_TUPLE(19, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,
                x17, x18)
DEFINE_AS_TUPLE(20, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,
                x17, x18, x19)
DEFINE_AS_TUPLE(21, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,
                x17, x18, x19, x20)
DEFINE_AS_TUPLE(22, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,
                x17, x18, x19, x20, x21)
DEFINE_AS_TUPLE(23, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,
                x17, x18, x19, x20, x21, x22)
DEFINE_AS_TUPLE(24, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,
                x17, x18, x19, x20, x21, x22, x23)
DEFINE_AS_TUPLE(25, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,
                x17, x18, x19, x20, x21, x22, x23, x24)
DEFINE_AS_TUPLE(26, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,
                x17, x18, x19, x20, x21, x22, x23, x24, x25)
DEFINE_AS_TUPLE(27, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,
                x17, x18, x19, x20, x21, x22, x23, x24, x25, x26)
DEFINE_AS_TUPLE(28, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,
                x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27)
DEFINE_AS_TUPLE(29, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,
                x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28)
DEFINE_AS_TUPLE(30, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,
                x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29)
DEFINE_AS_TUPLE(31, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,
    x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30)
DEFINE_AS_TUPLE(32, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16,
                x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31)
// clang-format on
}  // namespace fallback

// We consider aggregated structures as containers
// use pre_impl to update ctx
template <typename T, std::enable_if_t<!ostreamable_v<T>, bool> = true>
inline std::ostream &operator<<(std::ostream &os, const pre_impl<T> &t)
{
    auto tup = util::fallback::as_tuple(
        t.inner(),
        std::integral_constant<size_t, util::fallback::count<T>()>{});
    os << "{" << names::get_type_name<T>() << " " << util::pre(tup, t.ctx())
       << "}";
    return os;
}

template <typename T>
struct pre_with_name
{
    pre_with_name(const T &t, const char *n) : t_(t), name_(n)
    {
    }

    const T &t_;
    const char *name_;
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const pre_with_name<T> &p)
{
    os << p.name_ << ": " << util::pre(p.t_);
    return os;
}

#define __UP_get_name(var) #var
#define PRE(var) util::pre_with_name(var, __UP_get_name(var))

}  // namespace util

#endif
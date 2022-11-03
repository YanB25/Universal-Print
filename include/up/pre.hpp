#pragma once
#ifndef UNIVERSAL_PRESENTATION_H_
#define UNIVERSAL_PRESENTATION_H_

#include <array>
#include <atomic>
#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

template <typename T, typename = void>
auto constexpr ostreamable_v = false;
template <typename T>
auto constexpr ostreamable_v<
    T,
    std::void_t<decltype(std::cout << std::declval<T>())>> = true;

namespace util
{
// Universal presentation:
// If an object of type T is ostream-able
// Then, pre<T> is also ostream-able
template <typename T>
class pre
{
public:
    pre(const T &t,
        bool verbose = false,
        size_t limit = std::numeric_limits<size_t>::max())
        : t_(t), v_(verbose), limit_(limit)
    {
    }
    size_t limit() const
    {
        return limit_;
    }
    bool verbose() const
    {
        return v_;
    }
    const T &inner() const
    {
        return t_;
    }

private:
    const T &t_;
    bool v_{false};
    size_t limit_{};
};

// If T is ostream-able
// We trivially make util::pre<T> ostream-able
template <typename T,
          std::enable_if_t<ostreamable_v<T>, bool> = true,
          std::enable_if_t<!std::is_array_v<T>, bool> = true>
inline std::ostream &operator<<(std::ostream &os, const pre<T> &t)
{
    os << t.inner();
    return os;
}

template <typename T, typename = void>
auto constexpr universally_presentable_v = false;
template <typename T>
auto constexpr universally_presentable_v<
    T,
    std::void_t<decltype(std::cout << util::pre(std::declval<T>()))>> = true;

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
            os << util::pre(*front_iter);
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
                                const pre<std_container_present_impl<T>> t)
{
    const auto &pre = t.inner();
    const auto &obj = pre.inner();

    size_t start_size = 0;
    size_t end_size = 0;
    size_t ommitted = 0;
    size_t output_size = 0;
    size_t obj_size = std::size(obj);
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
        os << util::pre(*front_iter);
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
            os << util::pre(*back_iter) << pre.sep() << " ";
        }
        else
        {
            os << util::pre(*back_iter);
        }
        back_iter--;
    }
    os << pre.rb();
    if (t.verbose())
    {
        os << " (sz: " << obj_size << ", ommitted " << ommitted << ")";
    }
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre<std::vector<T>> &vec)
{
    os << util::pre(std_container_present_impl(vec.inner(), '[', ']', ','),
                    vec.verbose(),
                    vec.limit());
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const pre<std::list<T>> &lst)
{
    os << util::pre(std_container_present_impl(lst.inner(), '[', ']', ','),
                    lst.verbose(),
                    lst.limit());
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre<std::forward_list<T>> &lst)
{
    os << util::pre(
        std_forward_container_present_impl(lst.inner(), '[', ']', ','),
        lst.verbose(),
        lst.limit());
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const pre<std::set<T>> &set)
{
    os << util::pre(std_container_present_impl(set.inner(), '{', '}', ','),
                    set.verbose(),
                    set.limit());
    return os;
}
template <typename K, typename V>
inline std::ostream &operator<<(std::ostream &os, const pre<std::map<K, V>> &m)
{
    os << util::pre(std_container_present_impl(m.inner(), '{', '}', ','),
                    m.verbose(),
                    m.limit());
    return os;
}
template <typename K, typename V>
inline std::ostream &operator<<(std::ostream &os,
                                const pre<std::unordered_map<K, V>> &m)
{
    os << util::pre(
        std_forward_container_present_impl(m.inner()), m.verbose(), m.limit());
    return os;
}
template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre<std::unordered_set<T>> &set)
{
    os << util::pre(std_forward_container_present_impl(set.inner()),
                    set.verbose(),
                    set.limit());
    return os;
}
template <typename T, size_t size>
inline std::ostream &operator<<(std::ostream &os,
                                const pre<std::array<T, size>> &p)
{
    os << util::pre(
        std_container_present_impl(p.inner()), p.verbose(), p.limit());
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const pre<std::deque<T>> &p)
{
    os << util::pre(
        std_container_present_impl(p.inner()), p.verbose(), p.limit());
    return os;
}

template <typename T,
          size_t size,
          std::enable_if_t<!std::is_same_v<T, char>, bool> = true>
using c_style_array = T[size];

template <typename T, size_t size>
inline std::ostream &operator<<(std::ostream &os,
                                const pre<c_style_array<T, size>> &p)
{
    auto a = util::pre(
        std_container_present_impl(p.inner()), p.verbose(), p.limit());
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

inline std::ostream &operator<<(std::ostream &os, const pre<char> &p)
{
    os << "'" << p.inner() << "'";
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre<std::optional<T>> &t)
{
    if (t.inner().has_value())
    {
        os << "some(" << util::pre(t.inner().value()) << ")";
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
    os << "atomic(" << util::pre(t.inner().load(std::memory_order_relaxed))
       << ")";
    return os;
}

template <typename T, typename U>
inline std::ostream &operator<<(std::ostream &os, const pre<std::pair<T, U>> &s)
{
    const auto &t = s.inner();
    os << "(" << util::pre(t.first) << ", " << util::pre(t.second) << ")";
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

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const pre<std::queue<T>> &s)
{
    const auto &t = s.inner();
    const auto &c = get_container(t);
    os << util::pre(c);
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const pre<std::stack<T>> &s)
{
    const auto &t = s.inner();
    const auto &c = get_container(t);
    os << util::pre(c);
    return os;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os,
                                const pre<std::priority_queue<T>> &s)
{
    const auto &t = s.inner();
    const auto &c = get_container(t);
    os << util::pre(c);
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

template <typename T, typename... Ubiqs>
constexpr auto count_r(size_t &sz, int) -> std::void_t<decltype(T{Ubiqs{}...})>
{
    sz = sizeof...(Ubiqs);
}

template <typename T, typename, typename... Ubiqs>
constexpr auto count_r(size_t &sz, float)
{
    count_r<T, Ubiqs...>(sz, 0);
}

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

template <typename T, std::enable_if_t<!ostreamable_v<T>, bool> = true>
inline std::ostream &operator<<(std::ostream &os, const pre<T> &t)
{
    auto tup = util::fallback::as_tuple(
        t.inner(),
        std::integral_constant<size_t, util::fallback::count<T>()>{});
    os << "{Unknown " << util::pre(tup) << "}";
    return os;
}

}  // namespace util

#endif
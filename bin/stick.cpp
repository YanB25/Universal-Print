#include <iostream>

#include "up/pre.hpp"

class Pin
{
public:
    Pin(int m) : m_(m)
    {
    }
    Pin() = delete;
    Pin(const Pin &) = delete;
    Pin(Pin &&) = delete;
    Pin &operator=(const Pin &) = delete;
    Pin &&operator=(Pin &&) = delete;
    int m() const
    {
        return m_;
    }

private:
    int m_;
};
std::ostream &operator<<(std::ostream &os, const Pin &p)
{
    os << "{Pin " << p.m() << "}";
    return os;
}

int main()
{
    std::queue<Pin> queue;
    for (int i = 5; i < 10; ++i)
    {
        queue.emplace(i);
    }

    constexpr std::array<int, 2> arr{1, 2};
    static_assert(arr[0] == 1);

    std::cout << util::pre(queue) << std::endl;
    return 0;
}
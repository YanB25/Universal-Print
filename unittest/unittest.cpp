#include "gtest/gtest.h"
#include "trait.h"
#include "up/pre.hpp"

template <typename T, std::enable_if_t<is_push_back_able_v<T>, bool> = true>
auto container(size_t size)
{
    T t;
    for (size_t i = 0; i < size; ++i)
    {
        t.push_back(i);
    }
    return t;
}

template <typename T, std::enable_if_t<is_push_able_v<T>, bool> = true>
auto container(size_t size)
{
    T t;
    for (size_t i = 0; i < size; ++i)
    {
        t.push(i);
    }
    return t;
}

auto container_str(size_t size)
{
    std::string ret = "[";
    for (size_t i = 0; i < size; ++i)
    {
        if (i + 1 == size)
        {
            ret += std::to_string(i);
        }
        else
        {
            ret += std::to_string(i) + ", ";
        }
    }
    return ret + "]";
}
auto map_container_str(size_t size)
{
    std::string ret = "{";
    for (size_t i = 0; i < size; ++i)
    {
        ret += "(" + std::to_string(i) + ", " + std::to_string(i) + ")";
        if (i + 1 != size)
        {
            ret += ", ";
        }
    }
    return ret + "}";
}
auto set_container_str(size_t size)
{
    std::string ret = "{";
    for (size_t i = 0; i < size; ++i)
    {
        ret += std::to_string(i);
        if (i + 1 != size)
        {
            ret += ", ";
        }
    }
    return ret + "}";
}

class PrimitiveTests : public ::testing::TestWithParam<int>
{
};

TEST_P(PrimitiveTests, Vector)
{
    size_t size = GetParam();
    auto c = container<std::vector<int>>(size);
    EXPECT_EQ(util::pre_str(c), container_str(size));
}

TEST_P(PrimitiveTests, List)
{
    size_t size = GetParam();
    auto c = container<std::list<int>>(size);
    EXPECT_EQ(util::pre_str(c), container_str(size));
}
// TEST_P(PrimitiveTests, ForwardList)
// {
//     size_t size = GetParam();
//     auto vec = container<std::forward_list<int>>(size);
//     EXPECT_EQ(util::pre_str(vec), container_str(size));
// }

TEST_P(PrimitiveTests, Queue)
{
    size_t size = GetParam();
    auto c = container<std::queue<int>>(size);
    EXPECT_EQ(util::pre_str(c), container_str(size));
}

TEST_P(PrimitiveTests, Stack)
{
    size_t size = GetParam();
    auto c = container<std::stack<int>>(size);
    EXPECT_EQ(util::pre_str(c), container_str(size));
}

INSTANTIATE_TEST_SUITE_P(MyTests,
                         PrimitiveTests,
                         ::testing::ValuesIn({0, 4, 10}));

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
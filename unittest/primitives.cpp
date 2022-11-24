#include "gtest/gtest.h"
#include "up/pre.hpp"

TEST(Primitives, Int)
{
    EXPECT_EQ(util::pre_str(5), "5");
}
TEST(Primitives, Char)
{
    EXPECT_EQ(util::pre_str('a'), "'a'");
    EXPECT_EQ(util::pre_str('D'), "'D'");
    EXPECT_EQ(util::pre_str('H'), "'H'");
}
TEST(Primitives, String)
{
    EXPECT_EQ(util::pre_str("hello world"), "\"hello world\"");
    EXPECT_EQ(util::pre_str(std::string("hello world")), "\"hello world\"");
}
TEST(Primitives, Ptr)
{
    EXPECT_EQ(util::pre_str(nullptr), "nullptr");
    EXPECT_EQ(util::pre_str((void *) 0xaabbccddeeff1122), "0xaabbccddeeff1122");
}
TEST(Primitives, Utilities)
{
    EXPECT_EQ(util::pre_str(std::nullopt), "nullopt");
    EXPECT_EQ(util::pre_str(std::make_optional(5)), "some(5)");

    EXPECT_EQ(util::pre_str(std::make_pair(1, 2)), "(1, 2)");
    EXPECT_EQ(util::pre_str(std::make_tuple(1, 2, 3)), "<1, 2, 3>");
}
TEST(Primitives, Boolean)
{
    EXPECT_EQ(util::pre_str(true), "true");
    EXPECT_EQ(util::pre_str(false), "false");
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
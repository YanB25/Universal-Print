#include "gtest/gtest.h"
#include "up/pre.hpp"

struct Person
{
    std::string name;
    int age;
};

std::ostream &operator<<(std::ostream &os, const Person &p)
{
    os << "{Person " << p.name << " age: " << p.age << "}";
    return os;
};

TEST(Customized, Single)
{
    EXPECT_EQ(util::pre_str(Person{.name = "A", .age = 30}),
              "{Person A age: 30}");
}
TEST(Customized, Vector)
{
    std::vector<Person> vec{Person{.name = "Alice", .age = 20},
                            Person{.name = "Bob", .age = 30}};
    EXPECT_EQ(util::pre_str(vec),
              "[{Person Alice age: 20}, {Person Bob age: 30}]");
}

// struct Anon
// {
//     int age;
//     char byte;
// };
// TEST(AnonTest, Single)
// {
//     EXPECT_EQ(util::pre_str(Anon{.age = 20, .byte = 'h'}), "{Anon <20,
//     'h'>}");
// }

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
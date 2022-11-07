#include <iostream>

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

struct A
{
    int a;
};
std::ostream &operator<<(std::ostream &os, const A &obj)
{
    os << "{A " << util::pre(obj.a) << "}";
    return os;
};

struct B
{
    A a;
    int b;
};
std::ostream &operator<<(std::ostream &os, const B &obj)
{
    os << "{B a: " << util::pre(obj.a) << ", b: " << util::pre(obj.b) << "}";
    return os;
};

int main()
{
    std::vector<int> vec{1, 2, 3, 4, 5};
    std::cout << util::pre(vec) << std::endl;

    std::cout << util::pre(vec, 2, 100, true) << std::endl;
    std::cout << util::pre(vec, 0, 100, false) << std::endl;

    std::cout << util::pre(5) << std::endl;
    Person p{.name = "Alice", .age = 20};
    std::cout << util::pre(p) << std::endl;
    // {Person Alice age: 20}

    std::vector<Person> persons{
        Person{.name = "A", .age = 25},
        Person{.name = "B", .age = 30},
        Person{.name = "C", .age = 45},
    };
    std::cout << util::pre(persons) << std::endl;
    // [{Person A age: 25}, {Person B age: 30}, {Person C age: 45}]
    std::cout << util::pre(persons, 1, 100, false) << std::endl;
    // [{Person A age: 25}, ...]

    std::map<std::string, Person> m;
    m["boss"] = Person{.name = "A", .age = 70};
    m["employee"] = Person{.name = "B", .age = 30};
    m["employee2"] = Person{.name = "C", .age = 30};
    m["employee4"] = Person{.name = "D", .age = 30};
    std::cout << "map: " << util::pre(m) << std::endl;
    std::cout << "map: " << util::pre(m, 0, 100, false) << std::endl;
    std::cout << "map: " << util::pre(m, 1, 100, false) << std::endl;
    std::cout << "map: " << util::pre(m, 2, 100, false) << std::endl;
    // {("boss", {Person A age: 70}), ("employee", {Person B age: 30})}

    std::vector<std::vector<Person>> matrix;
    matrix.push_back(
        {Person{.name = "A", .age = 10}, Person{.name = "B", .age = 20}});
    matrix.push_back(
        {Person{.name = "C", .age = 30}, Person{.name = "D", .age = 40}});
    std::cout << "matrix: " << util::pre(matrix) << std::endl;
    // [[{Person A age: 10}, {Person B age: 20}], [{Person C age: 30}, {Person D
    // age: 40}]]
    std::cout << "matrix(3): " << util::pre(matrix, 100, 3) << std::endl;
    // matrix(3): [[{Person A age: 10}, {Person B age: 20}], [{Person C age:
    // 30}, {Person D age: 40}]]
    std::cout << "matrix(2): " << util::pre(matrix, 100, 2) << std::endl;
    // matrix(2): [[..., ...], [..., ...]]
    std::cout << "matrix(1): " << util::pre(matrix, 100, 1) << std::endl;
    // matrix(1): [..., ...]
    std::cout << "matrix(0): " << util::pre(matrix, 100, 0) << std::endl;

    std::pair<int, int> pair{100, 200};
    std::cout << "pair: " << util::pre(pair) << std::endl;
    // pair: (100, 200)

    std::tuple<Person, int, int, int> t{
        Person{.name = "tuple", .age = 4}, 2, 3, 4};
    std::cout << "tuple: " << util::pre(t) << std::endl;
    // tuple: <{Person tuple age: 4}, 2, 3, 4>

    std::forward_list<Person> fl;
    fl.push_front(Person{.name = "A", .age = 1});
    fl.push_front(Person{.name = "B", .age = 1});
    fl.push_front(Person{.name = "C", .age = 1});
    std::cout << "forward_list: " << util::pre(fl) << std::endl;
    // forward_list: [{Person B age: 1}, {Person A age: 1}]
    std::cout << "forward_list: " << util::pre(fl, 1, 100, false) << std::endl;
    // forward_list(omit): [{Person B age: 1}, ...]
    std::cout << "forward_list: " << util::pre(fl, 2, 100, false) << std::endl;
    // forward_list: [{Person C age: 1}, {Person B age: 1}, ...]
    std::cout << "forward_list: " << util::pre(fl, 0, 100, false) << std::endl;
    // forward_list: [...]

    int c_array[] = {2, 4, 6, 8, 10};
    std::cout << "c style array: " << util::pre(c_array) << std::endl;
    // c style array: [2, 4, 6, 8, 10]
    std::cout << "c style array: " << util::pre(c_array, 1, 100, false)
              << std::endl;
    // c style array: [2, ...]
    std::cout << "c style array: " << util::pre(c_array, 2, 100, false)
              << std::endl;
    // c style array: [2, ..., 8]
    std::cout << "c style array: " << util::pre(c_array, 0, 100, false)
              << std::endl;
    // c style array: [...]

    std::cout << "c style string: " << util::pre("hello world") << std::endl;
    // c style string: "hello world"
    std::cout << "c style ptr: " << util::pre((void *) 1024) << std::endl;
    // c style ptr: 0x400
    std::cout << "char: " << util::pre('a') << std::endl;
    // char: 'a'
    std::cout << "boolean: " << util::pre(true) << std::endl;
    // boolean: true

    A obj_a{.a = 5};
    B obj_b{.a = obj_a, .b = 10};
    std::cout << "B: " << obj_b << std::endl;

    {
        struct S
        {
            int x;
            std::string s;
            std::vector<int> v;
            std::map<int, int> m;
            std::atomic<int> a;
            bool b;
        };
        S s = {42, "42", {1, 2, 3}, {{5, 2}, {7, 11}}, 10, false};
        std::cout << util::pre(s) << std::endl;
        // {Unknown <42, "42", [1, 2, 3], {(5, 2), (7, 11)}, atomic(10), false>}
    }

    {
        std::queue<int> q;
        for (size_t i = 0; i < 4; ++i)
        {
            q.push(i);
        }
        std::cout << util::pre(q) << std::endl;
    }
    {
        std::byte a{'h'};
        std::cout << util::pre(a) << std::endl;
    }

    int hei_you = 5;
    std::cout << PRE(hei_you) << std::endl;

    return 0;
}
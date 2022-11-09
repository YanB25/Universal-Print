# Universal Print: A Header-only C++ Library to cout STL Containers and More

This library, UP (Universal Print), helps you to `std::cout` most C++ STL containers in addition to the primitive types, as long as you make the underlying object `T` *cout-able*.

For those not overloading `operator<<`, UP generates default outputs with the best effort, which works well in most cases.

The header is [here](include/up/pre.hpp).

## Usage

### Primitive Types

UP can print containers of primitive types with customized control.

``` c++
std::vector<int> vec{1, 2, 3, 4, 5};
std::cout << util::pre(vec) << std::endl;
// [1, 2, 3, 4, 5]

std::cout << util::pre(vec, 2 /* limit */) << std::endl;
// [1, ..., 4] (sz: 5, ommitted 3)

std::cout << util::pre(vec, 0 /* limit */) << std::endl;
// [...]
```

UP works with adaptors, e.g., `std::queue`, `std::stack`, without data movement.

``` c++
std::queue<int> queue; // assume it has [1, 2, 3, 4]
std::cout << "queue:" << util::pre(queue) << std::endl
// queue: [0, 1, 2, 3]
// ** YES, UP works with queue.
```

### Custom *Cout-able* Types

UP can handle containers of custom types as long as you make it *cout-able*.

For example, `Person` is a cout-able type that overloads `operator<<` in a standard way.

``` c++
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
```

In this case, UP handles the output of most STL containers for you automatically.

For vector (array, list, forward_list, etc)

``` c++
std::vector<Person> persons{
    Person{.name = "A", .age = 25},
    Person{.name = "B", .age = 30},
    Person{.name = "C", .age = 45},
};
std::cout << util::pre(persons) << std::endl;
// [{Person A age: 25}, {Person B age: 30}, {Person C age: 45}]

```

For map (unordered_map, unordered_multimap, set, unordered_set, unordered_multiset, etc)

``` c++
std::map<std::string, Person> m;
m["boss"] = Person{.name = "A", .age = 70};
m["employee"] = Person{.name = "B", .age = 30};
m["employee2"] = Person{.name = "C", .age = 30};
m["employee4"] = Person{.name = "D", .age = 30};
std::cout << "map: " << util::pre(m) << std::endl;
// map: {("boss", {Person A age: 70}), ("employee", {Person B age: 30}), ("employee2", {Person C age: 30}), ("employee4", {Person D age: 30})}
```

For two-dimention vector (multi-dimentional array, C-style array, etc)

``` c++
std::vector<std::vector<Person>> matrix;
matrix.push_back(
    {Person{.name = "A", .age = 10}, Person{.name = "B", .age = 20}});
matrix.push_back(
    {Person{.name = "C", .age = 30}, Person{.name = "D", .age = 40}});
std::cout << "matrix: " << util::pre(matrix) << std::endl;
// [[{Person A age: 10}, {Person B age: 20}], [{Person C age: 30}, {Person D age: 40}]]
```

### Not *Cout-able* Types: Best Effort Output

For the not cout-able types, UP provides a best-effort output.

``` c++
struct Obj
{
    int x;
    std::string s;
    std::vector<int> v;
    std::map<int, int> m;
    std::atomic<int> a;
    bool b;
};
// ** No operator<< overloaded.

Obj obj = {42, "42", {1, 2, 3}, {{5, 2}, {7, 11}}, 10, false};
std::cout << util::pre(obj) << std::endl;
// {Obj <42, "42", [1, 2, 3], {(5, 2), (7, 11)}, atomic(10), false>}
```

UP is able to retrieve the typename, i.e., `Obj`.

*Limitations*:

The labels of the class are wiped out. UP is unable to preserve them since C++ does not support [reflection](https://en.cppreference.com/w/cpp/keyword/reflexpr).

This feature heavily relies on [structured binding (C++17)](https://en.cppreference.com/w/cpp/language/structured_binding). Therefore, it does not recognize

- C-style array
- std::optional
- ... (maybe more)

Solve compile failure by explicitly overloading `operator<<` in this case.

This feature borrowed codes from [ezprint](https://github.com/Sinacam/ezprint).

### Other STL Wrappers and C-style Types

UP gives more friendly outputs to other STL utilities/wrappers and C-style types.

For pair and tuple:

``` c++
std::pair<int, int> pair{100, 200};
std::cout << "pair: " << util::pre(pair) << std::endl;
// pair: (100, 200)

std::tuple<Person, int, int, int> t{
    Person{.name = "tuple", .age = 4}, 2, 3, 4};
std::cout << "tuple: " << util::pre(t) << std::endl;
// tuple: <{Person tuple age: 4}, 2, 3, 4>
```

UP handles `std::tuple<Types...>` well.


UP recognizes C-style array, but does not mess up with C-style string:

``` c++
int c_array[] = {2, 4, 6, 8, 10};
std::cout << "c style array: " << util::pre(c_array) << std::endl;
// c style array: [2, 4, 6, 8, 10]
std::cout << "c style string: " << util::pre("hello world") << std::endl;
// c style string: "hello world"
// ** NOTE: will not show the ugly ['h', 'e', 'l', ...] version.

```

UP gives more friendly outputs to the primitive type.

``` c++
std::cout << "c style string: " << util::pre("hello world") << std::endl;
// c style string: "hello world"
std::cout << "char: " << util::pre('a') << std::endl;
// char: 'a'
std::cout << "boolean: " << util::pre(true) << std::endl;
// boolean: true
```

### With Variable Name

UP provides a similar `PRE` to show variable name.

``` c++
int hey_you = 5;
std::cout << PRE(hei_you) << std::endl;
// hei_you: 5
```

### Ensurance of No Data Movement

UP avoids any data movement, i.e., no copy and no move occurs.

Consider a non-copyable and non-movable type called `Pin`.

``` c++
class Pin
{
    // All copy and move deleted
    Pin() = delete;
    Pin(const Pin &) = delete;
    Pin(Pin &&) = delete;
    Pin &operator=(const Pin &) = delete;
    Pin &&operator=(Pin &&) = delete;
};
// with operator<< overloaded
```

UP is able to print it even if it is stored in some non-iterable adaptors, e.g., `std::queue`.

``` c++
std::queue<Pin> queue;
for (int i = 5; i < 10; ++i)
{
    queue.emplace(i); // in-place construction
}
std::cout << util::pre(queue) << std::endl; // no copy occurred
// [{Pin 5}, {Pin 6}, {Pin 7}, {Pin 8}, {Pin 9}]
```

### Applicability

UP supports most, if not all, STL wrappers. Including

- std::optional, std:: atomic
- std::shared_ptr, std::unique_ptr
- The adaptors, e.g., std::stack, std::queue, std::priority_queue, ... Yes, UP supports them without any *copy*.

Feature requests are welcomed.

For getting a `std::string`, UP provides a similar `util::pre_str`.

NOTE: the output of `std::vector<bool>` is platform-specific, due to the [specialization of bool vector](https://en.cppreference.com/w/cpp/container/vector_bool) in STL specification.

## Use UP Universally

We recommend to use UP universally, i.e., use it under any interactions with `std::ostream`.
Image you have nested classes, `A` and `B`.

``` c++
struct A
{
    int a;
};

struct B
{
    A a; // nested here
    int b;
};

```

Use UP to help you show *any* members recursively.

``` c++
std::ostream &operator<<(std::ostream &os, const A &obj)
{
    // use util::pre for int
    os << "{A " << util::pre(obj.a) << "}";
    return os;
};

std::ostream &operator<<(std::ostream &os, const B &obj)
{
    // use util::pre for A and int
    os << "{B a: " << util::pre(obj.a) << ", b: " << util::pre(obj.b) << "}";
    return os;
};
```

## Overhead

UP guarantees $O(n)$ overhead to any containers with $n$ elements. No copy is performed.

However, UP is optimized for human-readability instead of performance. Use it out of critical path for any performance-critical programs.

## Compile

``` bash
mkdir build; cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
# or cmake -DCMAKE_BUILD_TYPE=Release ..
# for more options, see CMakeLists.txt
make -j
```

## Run

``` bash
$ ./bin/main
```

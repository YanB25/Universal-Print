#pragma once
#include "up/pre.hpp"

template <typename T, typename = void>
auto constexpr is_push_back_able_v = false;
template <typename T>
auto constexpr is_push_back_able_v<
    T,
    std::void_t<decltype(std::declval<T>().push_back({}))>> = true;

template <typename T, typename = void>
auto constexpr is_push_able_v = false;
template <typename T>
auto constexpr is_push_able_v<
    T,
    std::void_t<decltype(std::declval<T>().push({}))>> = true;

template <typename T, typename = void>
auto constexpr is_push_front_able_v = false;
template <typename T>
auto constexpr is_push_front_able_v<
    T,
    std::void_t<decltype(std::declval<T>().push({}))>> = true;

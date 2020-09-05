#ifndef COMMON_H
#define COMMON_H

// TODO: make it portable.
// #if !define __builtin_expect
// #define __builtin_expect(x, expected_value) (x)
// #pragma message("No __builtin_expect defined.")
// #endif

#if !defined(likely)
#define likely(x) (__builtin_expect(!!(x), 1))
#endif

#if !defined(unlikely)
#define unlikely(x) (__builtin_expect(!!(x), 0))
#endif

#endif
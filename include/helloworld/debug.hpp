/**
 * @file this file define the macros used to use or assert
 *
 * There's multiple level of logging:
 * - info(...)
 * - warn(...)
 * - error(...)
 * - panic(...) which terminates the program after logging.
 *
 * There's also a debug version prefixed with `d` for each logging macros, e.g.
 * - dinfo(...)
 * - dpanic(...)
 * They are ignored if -DNDEBUG is an compile options.
 */
#ifndef CHECK_H
#define CHECK_H
#include <cstdio>
#include <exception>

#include "./common.hpp"

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

#define stderr_print(level, COLOR, M, ...)           \
    do                                               \
    {                                                \
        const char *__level = level;                 \
        fprintf(stderr,                              \
                COLOR "%s" RESET " %s:%d - " M "\n", \
                __level,                             \
                __FILE__,                            \
                __LINE__,                            \
                ##__VA_ARGS__);                      \
    } while (0)

#define info(M, ...) stderr_print("[Info] ", GRN, M, ##__VA_ARGS__)
#define warn(M, ...) stderr_print("[Warn] ", YEL, M, ##__VA_ARGS__)
#define error(M, ...) stderr_print("[Error]", RED, M, ##__VA_ARGS__)
#define panic(M, ...)                                                    \
    do                                                                   \
    {                                                                    \
        stderr_print("Panic", RED, M, ##__VA_ARGS__);                    \
        stderr_print(                                                    \
            "Panic", RED, "Program terminated due to the error above."); \
        std::terminate();                                                \
    } while (0)

// below is the `if` version
#define info_if(cond, ...) \
    if (cond)              \
    {                      \
        info(__VA_ARGS__); \
    }
#define warn_if(cond, ...) \
    if (cond)              \
    {                      \
        warn(__VA_ARGS__); \
    }
#define error_if(cond, ...) \
    if (unlikely(cond))     \
    {                       \
        error(__VA_ARGS__); \
    }
#define panic_if(cond, ...) \
    if (unlikely(cond))     \
    {                       \
        panic(__VA_ARGS__); \
    }

/**
 * @brief check the condition and print if it's violated
 */
#define check(cond, ...)          \
    do                            \
    {                             \
        const bool __cond = cond; \
        if (unlikely(!__cond))    \
        {                         \
            panic(__VA_ARGS__);   \
        }                         \
    } while (0)

#ifndef NDEBUG
// Debug mode
#define dcheck(cond, ...) check(cond, ##__VA_ARGS__)
#define dinfo(...) info(__VA_ARGS__)
#define dwarn(...) warn(__VA_ARGS__)
#define derror(...) error(__VA_ARGS__)
#define dpanic(...) panic(__VA_ARGS__)

#define dinfo_if(...) info_if(__VA_ARGS__)
#define dwarn_if(...) warn_if(__VA_ARGS__)
#define derror_if(...) error_if(__VA_ARGS__)
#define dpanic_if(...) panic_if(__VA_ARGS__)

#else
// Release Mode
#define dcheck(cond, ...)
#define dinfo(...)
#define dwarn(...)
#define derror(...)
#define dpanic(...)

#define dinfo_if(...)
#define dwarn_if(...)
#define derror_if(...)
#define dpanic_if(...)
#endif
#endif
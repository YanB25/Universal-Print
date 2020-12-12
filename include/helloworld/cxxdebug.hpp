#ifndef CXXDEBUG_H
#define CXXDEBUG_H
#include <inttypes.h>

#include <iostream>

#ifndef NDEBUG
constexpr bool is_debug = true;
#else
constexpr bool is_debug = false;
#endif

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

// This class is used to explicitly ignore values in the conditional
// logging macros.  This avoids compiler warnings like "value computed
// is not used" and "statement has no effect".
class LogMessageVoidify
{
public:
    LogMessageVoidify()
    {
    }
    // This has to be an operator with a precedence lower than << but
    // higher than ?:
    void operator&(std::ostream &)
    {
    }
};

static constexpr int INFO = 0, WARNING = 1, ERROR = 2, FATAL = 3;
static constexpr const char *severity_level[] = {
    "INFO", "WARNING", "ERROR", "PANIC"};
static constexpr const char *color[] = {GRN, YEL, RED, RED};
static constexpr const char *color_reset = RESET;

class Stream
{
public:
    Stream(const int severity, const char *file, size_t line)
        : severity_(severity), file_(file), line_(line)
    {
    }
    std::ostream &stream()
    {
        return std::cerr << color[severity_] << "[" << severity_level[severity_]
                         << "] " << color_reset << file_ << ":" << line_ << " ";
    }
    ~Stream()
    {
        std::cerr << std::endl;
        if constexpr (is_debug)
        {
            if (severity_ == FATAL)
            {
                std::terminate();
            }
        }
    }

private:
    int severity_;
    const char *file_;
    size_t line_;
};

// Use macro expansion to create, for each use of LOG_EVERY_N(), static
// variables with the __LINE__ expansion as part of the variable name.
#define LOG_EVERY_N_VARNAME(base, line) LOG_EVERY_N_VARNAME_CONCAT(base, line)
#define LOG_EVERY_N_VARNAME_CONCAT(base, line) base##line

#define LOG(severity) Stream(severity, __FILE__, __LINE__).stream()
#define LOG_IF(severity, cond) \
    (!cond) ? void(0) : LogMessageVoidify() & LOG(severity)

#define LOG_OCCURRENCES LOG_EVERY_N_VARNAME(occurrences_, __LINE__)
#define LOG_OCCURRENCES_MOD_N LOG_EVERY_N_VARNAME(occurrences_mod_n_, __LINE__)

#define LOG_EVERY_N(severity, n)             \
    static size_t LOG_OCCURRENCES_MOD_N = 0; \
    if (++LOG_OCCURRENCES_MOD_N > n)         \
        LOG_OCCURRENCES_MOD_N -= n;          \
    if (LOG_OCCURRENCES_MOD_N == 1)          \
    LOG(severity)

#define LOG_IF_EVERY_N(severity, cond, n)                                     \
    static size_t LOG_OCCURRENCES_MOD_N = 0;                                  \
    if (cond && ((LOG_OCCURRENCES_MOD_N = (LOG_OCCURRENCES_MOD_N + 1) % n) == \
                 (1 % n)))                                                    \
    LOG(severity)

#define LOG_FIRST_N(severity, n)         \
    static size_t LOG_OCCURRENCES_N = 0; \
    if (++LOG_OCCURRENCES_N <= n)        \
    LOG(severity)

#define CHECK_EQ(lhs, rhs) DO_CHECK(lhs, ==, rhs)
#define CHECK_NE(lhs, rhs) DO_CHECK(lhs, !=, rhs)
#define CHECK_GE(lhs, rhs) DO_CHECK(lhs, >=, rhs)
#define CHECK_LE(lhs, rhs) DO_CHECK(lhs, <=, rhs)
#define CHECK_GT(lhs, rhs) DO_CHECK(lhs, >, rhs)
#define CHECK_LT(lhs, rhs) DO_CHECK(lhs, <, rhs)
#define CHECK(cond)                                      \
    if (!cond)                                           \
    LOG(FATAL) << "Check Failed for cond: Expect: true " \
               << ", Actual: " << #cond << " is false. "

#define CHECK_VARNAME(base, line) CHECK_VARNAME_CONCAT(base, line)
#define CHECK_VARNAME_CONCAT(base, line) base##line

#define LHS_VAR CHECK_VARNAME(check_var_lhs_, __LINE__)
#define RHS_VAR CHECK_VARNAME(check_var_rhs_, __LINE__)

#define DO_CHECK(lhs, op, rhs)                                               \
    decltype(lhs) LHS_VAR = (lhs);                                           \
    decltype(rhs) RHS_VAR = (rhs);                                           \
    if (!(LHS_VAR op RHS_VAR))                                               \
    LOG(FATAL) << "Check Failed for cond: Expect: " << #lhs " " #op " " #rhs \
               << ", Actual: " << LHS_VAR << " vs " << RHS_VAR << ". "

#define SHOW(x) LOG(INFO) << "\"" #x "\": " << x

#ifdef NDEBUG
// release

#define YBENCH_EAT \
    static_cast<void>(0), true ? (void) 0 : LogMessageVoidify() & std::cerr

#define DLOG(severity) YBENCH_EAT
#define DLOG_EVERY_N(severity, n) YBENCH_EAT
#define DLOG_IF(severity, cond) YBENCH_EAT
#define DLOG_IF_EVERY_N(severity, cond, n) YBENCH_EAT
#define DLOG_FIRST_N(severity, n) YBENCH_EAT

#define DCHECK_EQ(lhs, rhs) YBENCH_EAT
#define DCHECK_NE(lhs, rhs) YBENCH_EAT
#define DCHECK_GE(lhs, rhs) YBENCH_EAT
#define DCHECK_LE(lhs, rhs) YBENCH_EAT
#define DCHECK_GT(lhs, rhs) YBENCH_EAT
#define DCHECK_LT(lhs, rhs) YBENCH_EAT
#define DCHECK(cond) YBENCH_EAT

#else
// debug
#define DLOG(severity) LOG(severity)
#define DLOG_EVERY_N(severity, n) LOG_EVERY_N(severity, n)
#define DLOG_IF(severity, cond) LOG_IF(severity, cond)
#define DLOG_IF_EVERY_N(severity, cond, n) LOG_IF_EVERY_N(severity, cond, n)
#define DLOG_FIRST_N(severity, n) LOG_FIRST_N(severity, n)

#define DCHECK_EQ(lhs, rhs) CHECK_EQ(lhs, rhs)
#define DCHECK_NE(lhs, rhs) CHECK_NE(lhs, rhs)
#define DCHECK_GE(lhs, rhs) CHECK_GE(lhs, rhs)
#define DCHECK_LE(lhs, rhs) CHECK_LE(lhs, rhs)
#define DCHECK_GT(lhs, rhs) CHECK_GT(lhs, rhs)
#define DCHECK_LT(lhs, rhs) CHECK_LT(lhs, rhs)
#define DCHECK(cond) CHECK(cond)

#endif

#endif
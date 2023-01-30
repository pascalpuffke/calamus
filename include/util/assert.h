#pragma once

#include <fmt/color.h>

#ifdef NDEBUG
#define ASSERT(cond) ((void)0)
#define ASSERT_MSG(cond, msg) ((void)0)
#else
#define ASSERT(cond) ({                                                                      \
    if (!(cond)) [[unlikely]] {                                                              \
        fmt::print(fmt::fg(fmt::terminal_color::bright_red),                                 \
            "ASSERTION FAILED: {}\n", #cond);                                                \
        fmt::print(fmt::fg(fmt::terminal_color::red), "\tin file:  {}:{}\n\tfunction: {}\n", \
            __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__);                                   \
        std::abort();                                                                        \
    }                                                                                        \
})

#define ASSERT_MSG(cond, msg) ({                                                             \
    if (!(cond)) [[unlikely]] {                                                              \
        fmt::print(fmt::fg(fmt::terminal_color::bright_red),                                 \
            "ASSERTION FAILED: {} ({})\n", msg, #cond);                                      \
        fmt::print(fmt::fg(fmt::terminal_color::red), "\tin file:  {}:{}\n\tfunction: {}\n", \
            __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__);                                   \
        std::abort();                                                                        \
    }                                                                                        \
})
#endif

#define UNREACHABLE() ASSERT_MSG(false, "Unreachable code reached")

#define TODO(msg) ASSERT_MSG(false, "TODO: " msg)

#define VERIFY_PTR(ptr) ({                        \
    ASSERT_MSG(ptr, "Expected non-null pointer"); \
    ptr;                                          \
})

#pragma once

#include <file/toml-config.h>
#include <fmt/color.h>
#include <resources/state.h>
#include <util/time.h>

// These require a populated config struct.
#define LOG_INFO(msg, ...)                                                                                     \
    {                                                                                                          \
        fmt::print(fmt::fg(fmt::terminal_color::white), "[{}] [{}:{}] info:\n",                                \
            calamus::Time::iso8601(), __PRETTY_FUNCTION__, __LINE__);                                          \
        fmt::print(fmt::emphasis::bold, "                           ↳ {}\n", fmt::format(msg, ##__VA_ARGS__)); \
    }
#define LOG_DEBUG(msg, ...)                                                            \
    {                                                                                  \
        if (state.config->debug) {                                                     \
            fmt::print(fmt::fg(fmt::terminal_color::cyan), "[{}] [{}:{}] debug:\n",    \
                calamus::Time::iso8601(), __PRETTY_FUNCTION__, __LINE__);              \
            fmt::print(fmt::emphasis::bold | fmt::fg(fmt::terminal_color::cyan),       \
                "                           ↳ {}\n", fmt::format(msg, ##__VA_ARGS__)); \
        }                                                                              \
    }
#define LOG_WARNING(msg, ...)                                                       \
    {                                                                               \
        fmt::print(fmt::fg(fmt::terminal_color::yellow), "[{}] [{}:{}] warning:\n", \
            calamus::Time::iso8601(), __PRETTY_FUNCTION__, __LINE__);               \
        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::terminal_color::yellow),      \
            "                           ↳ {}\n", fmt::format(msg, ##__VA_ARGS__));  \
    }
#define LOG_ERROR(msg, ...)                                                        \
    {                                                                              \
        fmt::print(fmt::fg(fmt::terminal_color::red), "[{}] [{}:{}] error:\n",     \
            calamus::Time::iso8601(), __PRETTY_FUNCTION__, __LINE__);              \
        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::terminal_color::red),        \
            "                           ↳ {}\n", fmt::format(msg, ##__VA_ARGS__)); \
    }

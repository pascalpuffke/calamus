#pragma once

#include <fmt/format.h>

/// Generates a fmt::formatter struct for given type.
/// Needs to be in the global namespace.
/// Only works for non-templated types.
#define FORMATTER(TYPE, FORMAT, ...)                                                            \
    template <>                                                                                 \
    struct fmt::formatter<TYPE> {                                                               \
        constexpr auto parse(fmt::format_parse_context& context) -> decltype(context.begin()) { \
            auto end = context.begin();                                                         \
            if (end != context.end() && (*++end) != '}')                                        \
                throw fmt::format_error("Invalid format");                                      \
            return end;                                                                         \
        }                                                                                       \
        template <typename FormatContext>                                                       \
        auto format(const TYPE& value, FormatContext& context) -> decltype(context.out()) {     \
            return format_to(context.out(), FORMAT, __VA_ARGS__);                               \
        }                                                                                       \
    };

#define DEFAULT_FORMAT_PARSE()                                 \
    constexpr auto parse(fmt::format_parse_context& context) { \
        auto end = context.begin();                            \
        if (end != context.end() && (*++end) != '}')           \
            throw fmt::format_error("Invalid format");         \
        return end;                                            \
    }

#pragma once

#include <fmt/format.h>

namespace calamus {

using fmt::print;

template <typename... T>
void println(fmt::format_string<T...> fmt = "", T&&... args) {
    const auto& vargs = fmt::make_format_args(args...);
    fmt::vprint(fmt, vargs);
    fmt::print("\n");
}

template <typename... T>
void eprint(fmt::format_string<T...> fmt, T&&... args) {
    const auto& vargs = fmt::make_format_args(args...);
    fmt::vprint(stderr, fmt, vargs);
}

template <typename... T>
void eprintln(fmt::format_string<T...> fmt = "", T&&... args) {
    const auto& vargs = fmt::make_format_args(args...);
    fmt::vprint(stderr, fmt, vargs);
    fmt::print(stderr, "\n");
}

}

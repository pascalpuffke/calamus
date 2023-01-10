#pragma once

#include <fmt/format.h>

namespace calamus {

template <typename T>
concept Formattable = fmt::is_formattable<T>::value;

using fmt::print;

void print(const Formattable auto& formattable_param) {
    fmt::print("{}", formattable_param);
}

template <typename... T>
void println(fmt::format_string<T...> fmt = "", T&&... args) {
    const auto& vargs = fmt::make_format_args(args...);
    fmt::vprint(fmt, vargs);
    fmt::print("\n");
}

void println(const Formattable auto& formattable_param) {
    println("{}", formattable_param);
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

void eprintln(const Formattable auto& formattable_param) {
    eprintln("{}", formattable_param);
}

}

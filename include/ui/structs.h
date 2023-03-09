#pragma once

#include <concepts>
#include <fmt/format.h>
#include <limits>
#include <util/common.h>
#include <util/typename.h>

namespace calamus {

template <typename T>
// 'bool' is considered an integral value (and thus arithmetic), which wouldn't make sense for our use cases.
concept Arithmetic = std::is_arithmetic_v<T> && !std::is_same_v<T, bool>;

enum class MouseButton : i32 {
    Left = 0,
    Right = 1,
    __Count = 2,
};

enum class MouseCursor : i32 {
    Default = 0,
};

template <Arithmetic T>
struct BasicColor {
    T r;
    T g;
    T b;
    T a;

    /// Constructs a Color from a 32-bit hexadecimal number (0xAARRGGBB).
    /// If alpha channel is 0, this function will return a=255. Use from_parts if you need to construct with a=0.
    static constexpr BasicColor<u8> from_hex(u32 hex) {
        const u8 alpha = (hex >> 24) & 0xFF;
        const u8 red = (hex >> 16) & 0xFF;
        const u8 green = (hex >> 8) & 0xFF;
        const u8 blue = hex & 0xFF;
        return { red, green, blue, static_cast<u8>(alpha ? alpha : 0xFF) };
    }

    static constexpr BasicColor<T> from_parts(T r, T g, T b, T a = std::numeric_limits<T>::max()) {
        return { r, g, b, a };
    }

    constexpr BasicColor() = default;
    constexpr BasicColor(const BasicColor&) = default;
    constexpr BasicColor(BasicColor&&) noexcept = default;

    constexpr BasicColor& operator=(const BasicColor&) = default;
    constexpr BasicColor& operator=(BasicColor&&) noexcept = default;

    // These operators are over/underflow-safe.
    constexpr auto operator+(const BasicColor<T>& other) const {
        constexpr auto max = std::numeric_limits<T>::max();
        return BasicColor<T> {
            check_overflow(max, r, other.r),
            check_overflow(max, g, other.g),
            check_overflow(max, b, other.b),
            check_overflow(max, a, other.a)
        };
    }
    constexpr auto operator-(const BasicColor<T>& other) const {
        constexpr auto min = std::numeric_limits<T>::min();
        return BasicColor<T> {
            check_underflow(min, r, other.r),
            check_underflow(min, g, other.g),
            check_underflow(min, b, other.b),
            check_underflow(min, a, other.a)
        };
    }
    constexpr auto operator+=(const BasicColor<T>& other) {
        constexpr auto max = std::numeric_limits<T>::max();
        r = check_overflow(max, r, other.r);
        g = check_overflow(max, g, other.g);
        b = check_overflow(max, b, other.b);
        a = check_overflow(max, a, other.a);
        return *this;
    }
    constexpr auto operator-=(const BasicColor<T>& other) {
        constexpr auto min = std::numeric_limits<T>::min();
        r = check_underflow(min, r, other.r);
        g = check_underflow(min, g, other.g);
        b = check_underflow(min, b, other.b);
        a = check_underflow(min, a, other.a);
        return *this;
    }

    constexpr auto operator<=>(const BasicColor&) const = default;

private:
    [[nodiscard]] constexpr T check_overflow(T max, T first, T second) const {
        if (second > (max - first))
            return max;
        return first + second;
    }

    [[nodiscard]] constexpr T check_underflow(T min, T first, T second) const {
        if (min > (first - second))
            return min;
        return first - second;
    }

    constexpr BasicColor(T n_r, T n_g, T n_b, T n_a)
        : r(n_r)
        , g(n_g)
        , b(n_b)
        , a(n_a) { }
};

using Color = BasicColor<u8>;

namespace default_palette {
    // clang-format off
    static constexpr const auto black      = Color::from_hex(0x000000);
    static constexpr const auto white      = Color::from_hex(0xFFFFFF);
    static constexpr const auto gray       = Color::from_hex(0x808080);
    static constexpr const auto dark_gray  = Color::from_hex(0x404040);
    static constexpr const auto light_gray = Color::from_hex(0xC8C8C8);
    static constexpr const auto red        = Color::from_hex(0xFF0000);
    static constexpr const auto green      = Color::from_hex(0x00FF00);
    static constexpr const auto blue       = Color::from_hex(0x0000FF);
    // clang-format on
}

template <Arithmetic T = i32>
struct Position {
    T x;
    T y;

    constexpr Position(T n_x, T n_y)
        : x(n_x)
        , y(n_y) {
    }
    constexpr explicit Position(T i)
        : x(i)
        , y(i) {
    }

    constexpr Position() = default;
    constexpr Position(const Position&) = default;
    constexpr Position(Position&&) noexcept = default;

    constexpr Position& operator=(const Position&) = default;
    constexpr Position& operator=(Position&&) noexcept = default;

    constexpr auto operator+(const Position& other) const { return Position { x + other.x, y + other.y }; }
    constexpr auto operator-(const Position& other) const { return Position { x - other.x, y - other.y }; }
    constexpr auto operator*(T other) const { return Position { x * other, y * other }; }
    constexpr auto operator/(T other) const { return Position { x / other, y / other }; }
    constexpr auto operator+=(const Position& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    constexpr auto operator-=(const Position& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr auto operator<=>(const Position&) const = default;
};

template <Arithmetic T = i32>
struct Size {
    T width;
    T height;

    constexpr Size(T n_width, T n_height)
        : width(n_width)
        , height(n_height) {
    }
    constexpr explicit Size(T square)
        : width(square)
        , height(square) {
    }

    constexpr Size() = default;
    constexpr Size(const Size&) = default;
    constexpr Size(Size&&) noexcept = default;

    constexpr Size& operator=(const Size&) = default;
    constexpr Size& operator=(Size&&) noexcept = default;

    constexpr auto operator+(const Size& other) const { return Size { width + other.width, height + other.height }; }
    constexpr auto operator-(const Size& other) const { return Size { width - other.width, height - other.height }; }
    constexpr auto operator*(const Size& other) const { return Size { width * other.width, height * other.height }; }
    constexpr auto operator/(const Size& other) const { return Size { width / other.width, height / other.height }; }
    constexpr auto operator*(T other) const { return Size { width * other, height * other }; }
    constexpr auto operator/(T other) const { return Size { width / other, height / other }; }
    constexpr auto operator+=(const Size& other) {
        width += other.width;
        height += other.height;
        return *this;
    }
    constexpr auto operator-=(const Size& other) {
        width -= other.width;
        height -= other.height;
        return *this;
    }

    constexpr auto operator<=>(const Size&) const = default;

    constexpr auto to_position() const { return Position<T> { width, height }; }
};

template <Arithmetic T = i32>
struct Rectangle {
    T x;
    T y;
    T width;
    T height;

    constexpr Rectangle() = default;
    constexpr Rectangle(const Rectangle&) = default;
    constexpr Rectangle(Rectangle&&) noexcept = default;
    constexpr Rectangle(T n_x, T n_y, T n_width, T n_height)
        : x(n_x)
        , y(n_y)
        , width(n_width)
        , height(n_height) {
    }
    template <Arithmetic PositionT = T, Arithmetic SizeT = T>
    constexpr Rectangle(Position<PositionT> pos, Size<SizeT> size)
        : x(pos.x)
        , y(pos.y)
        , width(size.width)
        , height(size.height) {
    }

    constexpr Rectangle& operator=(const Rectangle&) = default;
    constexpr Rectangle& operator=(Rectangle&&) noexcept = default;

    constexpr auto operator+(const Rectangle& other) const {
        return Rectangle { x + other.x, y + other.y, width + other.width, height + other.height };
    }
    constexpr auto operator-(const Rectangle& other) const {
        return Rectangle { x - other.x, y - other.y, width - other.width, height - other.height };
    }
    constexpr auto operator*(T other) const {
        return Rectangle { x * other, y * other, width * other, height * other };
    }
    constexpr auto operator/(T other) const {
        return Rectangle { x / other, y / other, width / other, height / other };
    }
    constexpr auto operator+=(const Rectangle& other) {
        x += other.x;
        y += other.y;
        width += other.width;
        height += other.height;
        return *this;
    }
    constexpr auto operator-=(const Rectangle& other) {
        x -= other.x;
        y -= other.y;
        width -= other.width;
        height -= other.height;
        return *this;
    }

    constexpr auto operator<=>(const Rectangle&) const = default;

    constexpr auto to_position() const { return Position { x, y }; }
    constexpr auto to_size() const { return Size { width, height }; }
};

using IntPosition = Position<i32>;
using IntSize = Size<i32>;
using IntRect = Rectangle<i32>;

}

template <calamus::Arithmetic T>
struct fmt::formatter<calamus::BasicColor<T>> {
    DEFAULT_FORMAT_PARSE()
    template <typename FormatContext>
    auto format(const calamus::BasicColor<T>& value, FormatContext& context) {
        return format_to(context.out(), "({}, {}, {}, {})", value.r, value.g, value.b, value.a);
    }
};

template <calamus::Arithmetic T>
struct fmt::formatter<calamus::Rectangle<T>> {
    DEFAULT_FORMAT_PARSE()
    template <typename FormatContext>
    auto format(const calamus::Rectangle<T>& value, FormatContext& context) {
        return format_to(context.out(), "({}, {}, {}, {})", value.x, value.y, value.width, value.height);
    }
};

template <calamus::Arithmetic T>
struct fmt::formatter<calamus::Position<T>> {
    DEFAULT_FORMAT_PARSE()
    template <typename FormatContext>
    auto format(const calamus::Position<T>& value, FormatContext& context) {
        return format_to(context.out(), "({}, {})", value.x, value.y);
    }
};

template <calamus::Arithmetic T>
struct fmt::formatter<calamus::Size<T>> {
    DEFAULT_FORMAT_PARSE()
    template <typename FormatContext>
    auto format(const calamus::Size<T>& value, FormatContext& context) {
        return format_to(context.out(), "({}x{})", value.width, value.height);
    }
};

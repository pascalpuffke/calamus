#pragma once

#include <assert.hpp>
#include <optional>
#include <util/formatter.h>
#include <util/typename.h>

namespace calamus {

template <typename F, typename T>
concept SameInvokeResult = std::is_same_v<std::remove_cvref_t<std::invoke_result_t<F>>, T>;

// Errors need to be constructed explicitly.
//     Result<int> return_error() {
//         return Error { "Something went wrong" }; // please never write error messages like this
//     }
// Positive results can be constructed implicitly.
//     Result<int> return_int() {
//         return 42;
//     }
// When there's no positive Result (void), return 'void' with the ::success() helper:
//     Result<void> maybe_error() {
//         if (some_event)
//             return Error { "Oh no" };
//         return Result<void>::success(); // equivalent to 'return {};'
//     }

/// An Error type that only holds a string message.
class Error {
public:
    constexpr explicit Error(std::string&& message)
        : m_message(std::move(message)) {
    }
    constexpr explicit Error(const char* message)
        : m_message(message) {
    }

    template <typename... T>
    constexpr static Error formatted(fmt::format_string<T...> fmt, T&&... args) {
        return Error {
            fmt::vformat(fmt, fmt::make_format_args(args...))
        };
    }

    constexpr ~Error() = default;
    constexpr Error(const Error&) = default;
    constexpr Error(Error&&) noexcept = default;

    constexpr bool operator==(const Error& other) const {
        return m_message == other.m_message;
    }

    [[nodiscard]] constexpr std::string_view message() const {
        return m_message;
    }

private:
    std::string m_message;
};

template <typename ValueType, typename ErrorType = Error>
class Result {
public:
    constexpr explicit(false) Result(const ValueType& value)
        : m_value(value) {
    }

    constexpr explicit(false) Result(ValueType&& value) noexcept
        : m_value(std::move(value)) {
    }

    constexpr explicit(false) Result(const ErrorType& error)
        : m_error(error) {
    }

    constexpr explicit(false) Result(ErrorType&& error) noexcept
        : m_error(std::move(error)) {
    }

    constexpr ~Result() = default;

    constexpr Result(Result&&) noexcept = default;
    constexpr Result(const Result&) = default;
    constexpr Result& operator=(Result&&) noexcept = default;
    constexpr Result& operator=(const Result&) = default;

    [[nodiscard]] constexpr bool has_value() const {
        return m_value.has_value();
    }
    [[nodiscard]] constexpr bool has_error() const {
        return m_error.has_value();
    }
    [[nodiscard]] constexpr explicit operator bool() const {
        return has_value();
    }

    [[nodiscard]] constexpr ValueType& value() & {
        VERIFY(has_value(), "Result has no value");
        return m_value.value();
    }

    [[nodiscard]] constexpr const ValueType& value() const& {
        VERIFY(has_value(), "Result has no value");
        return m_value.value();
    }

    /**
     * This function releases ownership of the underlying Value.
     * After calling this function, has_value() will return false and any further operations are undefined behaviour.
     */
    [[nodiscard]] constexpr ValueType release_value() {
        VERIFY(has_value(), "Result has no value");
        auto moved = std::move(m_value.value());
        value().~ValueType();
        m_value.reset();
        return moved;
    }

    template <typename U>
        requires std::is_convertible_v<U&&, ValueType> && std::is_copy_constructible_v<ValueType>
    [[nodiscard]] constexpr ValueType value_or(U&& default_value) const& {
        if (has_error())
            return default_value;
        return m_value.value();
    }

    template <std::invocable Function>
        requires SameInvokeResult<Function, ValueType>
    constexpr ValueType value_or_else(Function&& func) {
        if (has_value())
            return release_value();
        return func();
    }

    [[nodiscard]] constexpr const ErrorType& error() const& {
        VERIFY(has_error(), "Result has no error");
        return m_error.value();
    }
    // I don't think it makes much sense to create more error accessors than const&

    template <typename ValueFn, typename ErrorFn>
    constexpr void match(ValueFn&& on_value, ErrorFn&& on_error) const {
        if (has_value())
            on_value(value());
        else
            on_error(error());
    }

    [[nodiscard]] constexpr ValueType& operator*() {
        return value();
    }

private:
    std::optional<ValueType> m_value {};
    std::optional<ErrorType> m_error {};
};

template <typename ErrorType>
class Result<void, ErrorType> {
public:
    constexpr Result()
        : m_error(std::nullopt) {
    }

    constexpr explicit(false) Result(const ErrorType& error)
        : m_error(error) {
    }
    constexpr explicit(false) Result(ErrorType&& error) noexcept
        : m_error(std::move(error)) {
    }

    constexpr ~Result() = default;

    constexpr Result(Result&&) noexcept = default;
    constexpr Result(const Result&) = default;
    constexpr Result& operator=(Result&&) noexcept = default;
    constexpr Result& operator=(const Result&) = default;

    static constexpr Result success() {
        return Result<void> {};
    }

    constexpr void release_value() { }

    [[nodiscard]] constexpr bool has_value() const {
        return false;
    }
    [[nodiscard]] constexpr bool has_error() const {
        return m_error.has_value();
    }
    [[nodiscard]] constexpr explicit operator bool() const {
        return !has_error();
    }

    [[nodiscard]] constexpr const ErrorType& error() const& {
        VERIFY(has_error(), "Result has no error");
        return m_error.value();
    }

private:
    std::optional<ErrorType> m_error {};
};

}

FORMATTER(calamus::Error, "{}", value.message())

template <typename ErrorT>
    requires fmt::is_formattable<ErrorT>::value
struct fmt::formatter<calamus::Result<void, ErrorT>> {
    DEFAULT_FORMAT_PARSE()

    template <typename FormatContext>
    auto format(const calamus::Result<void, ErrorT>& result, FormatContext& context) -> decltype(context.out()) {
        if (!result)
            return fmt::format_to(context.out(), "{}", result.error());
        return fmt::format_to(context.out(), "<void result>");
    }
};

template <typename ValueT, typename ErrorT>
    requires fmt::is_formattable<ValueT>::value && fmt::is_formattable<ErrorT>::value
struct fmt::formatter<calamus::Result<ValueT, ErrorT>> {
    DEFAULT_FORMAT_PARSE()

    template <typename FormatContext>
    auto format(const calamus::Result<ValueT, ErrorT>& result, FormatContext& context) -> decltype(context.out()) {
        if (result)
            return fmt::format_to(context.out(), "{}", result.value());
        return fmt::format_to(context.out(), "{}", result.error());
    }
};

// ✨ Inspired by SerenityOS ✨
#define TRY(expr) ({                           \
    auto __try_result = (expr);                \
    if (__try_result.has_error()) [[unlikely]] \
        return __try_result.error();           \
    __try_result.release_value();              \
})

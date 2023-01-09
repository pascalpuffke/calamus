#pragma once

#include <optional>
#include <util/assert.h>
#include <util/formatter.h>

namespace calamus {

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
    explicit Error(std::string&& message)
        : m_message(std::move(message)) {
    }
    explicit Error(const char* message)
        : m_message(message) {
    }

    template <typename... T>
    static Error formatted(fmt::format_string<T...> fmt, T&&... args) {
        return Error {
            fmt::vformat(fmt, fmt::make_format_args(args...))
        };
    }

    ~Error() = default;
    Error(const Error&) = default;
    Error(Error&&) noexcept = default;

    bool operator==(const Error& other) const {
        return m_message == other.m_message;
    }

    [[nodiscard]] const std::string& message() const {
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

    constexpr Result(Result&& other) noexcept = default;
    constexpr Result(const Result& other) = default;

    [[nodiscard]] constexpr bool has_value() const { return m_value.has_value(); }
    [[nodiscard]] constexpr bool has_error() const { return m_error.has_value(); }

    [[nodiscard]] constexpr ValueType& value() {
        ASSERT_MSG(!has_error(), "Result has no value");
        return m_value.value();
    }

    [[nodiscard]] constexpr ValueType& value_or(ValueType&& default_value) {
        if (has_error())
            return default_value;
        return m_value.value();
    }

    [[nodiscard]] constexpr ErrorType& error() {
        ASSERT_MSG(has_error(), "Result has no error");
        return m_error.value();
    }

    template <typename Function>
    constexpr ValueType value_or_else(Function&& func) {
        if (has_value())
            return std::move(value());
        return std::move(func());
    }

    template <typename ValueFn, typename ErrorFn>
    constexpr void match(ValueFn&& on_value, ErrorFn&& on_error) {
        if (has_value())
            on_value(value());
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

    constexpr Result(Result&& other) noexcept = default;
    constexpr Result(const Result& other) = default;

    static constexpr Result success() {
        return Result<void> {};
    }

    constexpr void value() { }

    [[nodiscard]] constexpr bool has_value() const { return !m_error.has_value(); }
    [[nodiscard]] constexpr bool has_error() const { return m_error.has_value(); }

    [[nodiscard]] ErrorType& error() {
        ASSERT_MSG(has_error(), "Result has no error");
        return m_error.value();
    }

private:
    std::optional<ErrorType> m_error {};
};

}

FORMATTER(calamus::Error, "Error(message='{}')", value.message())

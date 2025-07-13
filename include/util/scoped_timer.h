#pragma once

#include <chrono>
#include <fmt/color.h>
#include <util/logging.h>
#include <util/types.h>

namespace calamus {

// clang-format off
template <typename T>
concept DurationLike = requires(T duration) {
    { duration.count() } -> std::convertible_to<f64>;
};
// clang-format on

class ScopedTimer final {
public:
    explicit ScopedTimer(std::string_view name)
        : m_name(name) {
        m_start = Clock::now();
    }

    ~ScopedTimer() {
        const DurationLike auto end = elapsed<std::chrono::microseconds>();
        const auto ms = static_cast<f64>(end.count()) / 1000.0;
        LOG_INFO("Timer '{}' finished in {} ms", fmt::styled(m_name, fmt::fg(fmt::color::gray)), ms);
    }

    template <DurationLike T>
    [[nodiscard]] constexpr T elapsed() const {
        return std::chrono::duration_cast<T>(Clock::now() - m_start);
    }

private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    TimePoint m_start;
    std::string_view m_name;
};

}

#define SCOPED_TIMER const auto _timer = ::calamus::ScopedTimer { __FUNCTION__ }

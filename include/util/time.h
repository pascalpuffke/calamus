#pragma once

#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <fmt/chrono.h>

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

#include <util/types.h>

namespace calamus {

class Time {
public:
    /// @brief Returns the current Unix epoch timestamp in seconds.
    static i64 now() {
        return std::time(nullptr);
    }

    /// @brief Returns the current time as a string in the ISO 8601-standard format "YYYY-mm-ddTHH:MM:SSz"
    static std::string iso8601() {
        return fmt::format("{:%Y-%m-%dT%H:%M:%S%z}", fmt::localtime(now()));
    }
};

}

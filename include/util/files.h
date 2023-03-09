#pragma once

#include <filesystem>
#include <util/common.h>
#include <vector>

namespace calamus {

class Files final {
public:
    using path = std::filesystem::path;

    [[nodiscard]] static auto size(const path&) -> Result<i64>;

    [[nodiscard]] static auto read_string(const path&) -> Result<std::string>;
    [[nodiscard]] static auto read_lines(const path&) -> Result<std::vector<std::string>>;
    [[nodiscard]] static auto read_bytes(const path&) -> Result<std::vector<u8>>;

    enum class WriteOption {
        Append,
        Overwrite
    };
    [[nodiscard]] static auto write_string(
        const path&,
        std::string_view string,
        WriteOption write_option = WriteOption::Overwrite) -> Result<void>;

    auto operator=(const Files&) = delete;
    auto operator=(Files&&) = delete;

    ~Files() = delete;
};

}
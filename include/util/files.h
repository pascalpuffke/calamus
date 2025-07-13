#pragma once

#include <filesystem>
#include <fstream>
#include <util/result.h>
#include <vector>

namespace calamus {

// Because error_if_stream_bad is called with both if- and of-stream types,
// let's overengineer a solution allowing this usage.
template <typename T>
concept Stream = requires(T stream) {
    { stream.is_open() } -> std::same_as<bool>;
    { stream.fail() } -> std::same_as<bool>;
    { stream.close() } -> std::same_as<void>;
};

class Files final {
public:
    using path = std::filesystem::path;

    [[nodiscard]] static auto error_if_not_exists(const path& path) -> Result<void> {
        if (!exists(path))
            return Error { "File does not exist" };
        return Result<void>::success();
    }

    [[nodiscard]] static auto error_if_not_regular_file(const path& path) -> Result<void> {
        if (!is_regular_file(path))
            return Error { "Not a regular file" };
        return Result<void>::success();
    }

    [[nodiscard]] static auto error_if_stream_bad(const Stream auto& stream) -> Result<void> {
        if (!stream.is_open())
            return Error { "Stream not open" };
        if (stream.fail()) // fail() returns true for both badbit and failbit
            return Error { "Broken stream, possible I/O error or missing permissions" };
        return Result<void>::success();
    }

    [[nodiscard]] static auto stream_size(std::ifstream& stream) -> Result<i64> {
        TRY(error_if_stream_bad(stream));

        const auto pos = stream.tellg();
        if (pos == std::ifstream::pos_type { -1 })
            return Error { "Failed to tell current stream position" };

        stream.seekg(0, std::ios_base::end);
        const auto size = stream.tellg();
        if (size == std::ifstream::pos_type { -1 })
            return Error { "Failed to tell end of stream position" };

        // Reset stream to original position
        stream.seekg(pos, std::ios_base::beg);
        if (stream.fail())
            return Error { "Failed to seek back to original stream position" };

        return static_cast<i64>(size);
    }

    [[nodiscard]] static auto file_size(const path& path) -> Result<i64> {
        TRY(error_if_not_exists(path));
        TRY(error_if_not_regular_file(path));

        auto stream = std::ifstream { path };

        return stream_size(stream);
    }

    [[nodiscard]] static auto read_file_as_string(const path& path) -> Result<std::string> {
        TRY(error_if_not_exists(path));
        TRY(error_if_not_regular_file(path));

        auto stream = std::ifstream { path, std::ios_base::in | std::ios_base::binary };
        auto size = TRY(stream_size(stream));
        auto string = std::string {};

        string.resize(static_cast<usize>(size));
        stream.read(string.data(), size);

        return string;
    }

    [[nodiscard]] static auto read_file_as_lines(const path& path) -> Result<std::vector<std::string>> {
        TRY(error_if_not_exists(path));
        TRY(error_if_not_regular_file(path));

        auto stream = std::ifstream { path, std::ios_base::in };
        auto result = std::vector<std::string> {};

        for (auto line = std::string {}; std::getline(stream, line);) {
            result.emplace_back(line);
        }

        return result;
    }

    [[nodiscard]] static auto read_file_as_bytes(const path& path) -> Result<std::vector<u8>> {
        TRY(error_if_not_exists(path));
        TRY(error_if_not_regular_file(path));

        auto stream = std::ifstream { path, std::ios_base::in | std::ios_base::binary };
        auto size = TRY(stream_size(stream));
        auto storage = std::vector<u8> {};

        // I don't like this
        storage.resize(static_cast<usize>(size));
        stream.read(reinterpret_cast<i8*>(storage.data()), size);

        return storage;
    }

    template <typename T>
    [[nodiscard]] static auto read_from_stream(std::ifstream& stream) -> Result<T> {
        TRY(error_if_stream_bad(stream));
        T value;
        stream.read(reinterpret_cast<char*>(&value), sizeof(T));
        if (!stream)
            return Error { "Failed to read value from stream" };
        return value;
    }

    /// Reads a little endian value from the stream.
    /// On big endian systems, the value is byte-swapped.
    template <typename T>
    [[nodiscard]] static auto read_little_endian_from_stream(std::ifstream& stream) -> Result<T> {
        auto value = TRY(read_from_stream<T>(stream));
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        if constexpr (sizeof(T) == 8)
            return __builtin_bswap64(value);
        else if constexpr (sizeof(T) == 4)
            return __builtin_bswap32(value);
        else if constexpr (sizeof(T) == 2)
            return __builtin_bswap16(value);
        else
            return value;
#else // Little endian
        return value;
#endif
    }

    /// Reads a big endian value from the stream.
    /// On little endian systems, the value is byte-swapped.
    template <typename T>
    [[nodiscard]] static auto read_big_endian_from_stream(std::ifstream& stream) -> Result<T> {
        auto value = TRY(read_from_stream<T>(stream));
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        if constexpr (sizeof(T) == 8)
            return __builtin_bswap64(value);
        else if constexpr (sizeof(T) == 4)
            return __builtin_bswap32(value);
        else if constexpr (sizeof(T) == 2)
            return __builtin_bswap16(value);
        else
            return value;
#else // Big endian
        return value;
#endif
    }

    template <typename T>
    [[nodiscard]] static auto read_file_as(const path& path) -> Result<T> {
        TRY(error_if_not_exists(path));
        TRY(error_if_not_regular_file(path));

        auto stream = std::ifstream { path, std::ios_base::in | std::ios_base::binary };
        return read_from_stream<T>(stream);
    }

    enum class WriteOption {
        Append,
        Overwrite
    };
    [[nodiscard]] static auto write_string(
        const path& path,
        std::string_view string,
        WriteOption write_option = WriteOption::Overwrite
    ) -> Result<void> {
        const auto mode = write_option == WriteOption::Append
            ? std::ofstream::app
            : std::ofstream::trunc;
        auto stream = std::ofstream { path, mode };
        TRY(error_if_stream_bad(stream));

        stream.write(string.data(), static_cast<i64>(string.size()));
        TRY(error_if_stream_bad(stream));

        return Result<void>::success();
    }

    auto operator=(const Files&) = delete;
    auto operator=(Files&&) = delete;

    ~Files() = delete;
};

}
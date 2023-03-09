#include <fstream>
#include <util/files.h>
#include <utility>

namespace calamus {

namespace fs = std::filesystem;

auto error_if_not_exists(const Files::path& path) -> Result<void> {
    if (!exists(path))
        return Error { "File does not exist" };
    return Result<void>::success();
}

auto error_if_not_regular_file(const Files::path& path) -> Result<void> {
    if (!is_regular_file(path))
        return Error { "Not a regular file" };
    return Result<void>::success();
}

auto error_if_no_perm(const Files::path& path, fs::perms perm) -> Result<void> {
    if ((status(path).permissions() & perm) == fs::perms::none)
        return Error::formatted("Permission denied ({:#o})", std::to_underlying(perm));
    return Result<void>::success();
}

template <typename T>
concept Stream = requires(T stream) {
    { stream.is_open() } -> std::same_as<bool>;
    { stream.bad() } -> std::same_as<bool>;
    { stream.close() } -> std::same_as<void>;
};

auto error_if_stream_bad(const Stream auto& stream) -> Result<void> {
    if (!stream.is_open())
        return Error { "Input file stream not open" };
    if (stream.bad())
        return Error { "Input file stream 'badbit' set" };
    return Result<void>::success();
}

auto stream_size(std::ifstream& stream) -> Result<i64> {
    TRY(error_if_stream_bad(stream));

    auto pos = stream.tellg();
    if (pos == std::ifstream::pos_type { -1 })
        return Error { "'tellg()' returned failure (-1)" };

    stream.seekg(0, std::ios_base::end);
    auto size = stream.tellg();

    // Reset stream to original position
    stream.seekg(pos, std::ios_base::beg);

    return static_cast<i64>(size);
}

auto Files::size(const Files::path& path) -> Result<i64> {
    TRY(error_if_not_exists(path));
    TRY(error_if_not_regular_file(path));
    TRY(error_if_no_perm(path, fs::perms::others_read));

    auto stream = std::ifstream { path };

    return stream_size(stream);
}

auto Files::read_string(const Files::path& path) -> Result<std::string> {
    TRY(error_if_not_exists(path));
    TRY(error_if_not_regular_file(path));
    TRY(error_if_no_perm(path, fs::perms::others_read));

    auto stream = std::ifstream { path, std::ios_base::in };
    auto size = TRY(stream_size(stream));
    auto string = std::string {};

    string.resize(static_cast<usize>(size));
    stream.read(string.data(), size);

    return string;
}

auto Files::read_lines(const Files::path& path) -> Result<std::vector<std::string>> {
    TRY(error_if_not_exists(path));
    TRY(error_if_not_regular_file(path));
    TRY(error_if_no_perm(path, fs::perms::others_read));

    auto stream = std::ifstream { path, std::ios_base::in };
    auto result = std::vector<std::string> {};

    for (auto line = std::string {}; std::getline(stream, line);) {
        result.emplace_back(line);
    }

    return result;
}

auto Files::read_bytes(const Files::path& path) -> Result<std::vector<u8>> {
    TRY(error_if_not_exists(path));
    TRY(error_if_not_regular_file(path));
    TRY(error_if_no_perm(path, fs::perms::others_read));

    auto stream = std::ifstream { path, std::ios_base::in | std::ios_base::binary };
    auto size = TRY(stream_size(stream));
    auto storage = std::vector<u8> {};

    // I don't like this
    storage.resize(static_cast<usize>(size));
    stream.read(reinterpret_cast<i8*>(storage.data()), size);

    return storage;
}

auto Files::write_string(const Files::path& path, std::string_view string, Files::WriteOption write_option) -> Result<void> {
    TRY(error_if_no_perm(path, fs::perms::others_write));

    const auto mode = write_option == WriteOption::Append
        ? std::ofstream::app
        : std::ofstream::trunc;
    auto stream = std::ofstream { path, mode };
    TRY(error_if_stream_bad(stream));

    stream.write(string.data(), static_cast<i64>(string.size()));
    TRY(error_if_stream_bad(stream));

    return Result<void>::success();
}

}
#pragma once

#include <fstream>
#include <util/types.h>

namespace calamus {

class Reader {
public:
    template <typename T>
    static T read(std::ifstream& file) {
        T value;
        file.read(reinterpret_cast<char*>(&value), sizeof(T));
        return value;
    }

    template <typename T>
    /// Reads a little endian value from the file.
    /// On big endian systems, the value is byte-swapped.
    [[maybe_unused]] static T read_little_endian(std::ifstream& file) {
        auto value = read<T>(file);
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

    template <typename T>
    /// Reads a big endian value from the file.
    /// On little endian systems, the value is byte-swapped.
    static T read_big_endian(std::ifstream& file) {
        auto value = read<T>(file);
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

    /// Reads a single byte from given file.
    /// This is an alias for read\<u8\>().
    static u8 read_byte(std::ifstream& file) {
        return read<u8>(file);
    }
};

}

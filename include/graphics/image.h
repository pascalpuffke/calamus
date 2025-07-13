#pragma once

#include <ui/structs.h>
#include <assert.hpp>

namespace calamus {

/**
 * Images are resources allocated on system memory.
 * For 2D textures, use the GPU-allocated `Texture` class.
 */
class Image final {
public:
    constexpr Image() = default;
    Image(
        void* data,
        IntSize size,
        i32 mipmaps,
        i32 format
    );
    ~Image();

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    Image(Image&&) noexcept = default;
    Image& operator=(Image&&) noexcept = default;

    [[nodiscard]] constexpr auto data() const noexcept -> void* {
        VERIFY(m_data, "Image has no data or is already freed");
        return m_data.get();
    }
    [[nodiscard]] constexpr auto size() const noexcept -> IntSize { return m_size; }
    [[nodiscard]] constexpr auto width() const noexcept -> i32 { return m_size.width; }
    [[nodiscard]] constexpr auto height() const noexcept -> i32 { return m_size.height; }
    [[nodiscard]] constexpr auto mipmaps() const noexcept -> i32 { return m_mipmaps; }
    [[nodiscard]] constexpr auto format() const noexcept -> i32 { return m_format; }

private:
    std::unique_ptr<u8> m_data { nullptr };
    IntSize m_size { 0 };
    i32 m_mipmaps { 0 };
    i32 m_format { 0 };
};

}
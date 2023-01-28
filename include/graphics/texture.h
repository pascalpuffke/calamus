#pragma once

#include <span>
#include <ui/structs.h>

namespace calamus {

class Texture final {
public:
    constexpr Texture() = default;
    Texture(std::span<u8> image_data, IntSize size, i32 mipmaps, i32 format);

    ~Texture();

    [[nodiscard]] constexpr auto id() const noexcept { return m_gl_id; }
    [[nodiscard]] constexpr auto size() const noexcept { return m_size; }
    [[nodiscard]] constexpr auto width() const noexcept { return m_size.width; }
    [[nodiscard]] constexpr auto height() const noexcept { return m_size.height; }
    [[nodiscard]] constexpr auto mipmaps() const noexcept { return m_mipmaps; }
    [[nodiscard]] constexpr auto format() const noexcept { return m_format; }

private:
    std::span<u8> m_image_data {};

    u32 m_gl_id {};
    IntSize m_size {};
    i32 m_mipmaps {};
    i32 m_format {};
};

}
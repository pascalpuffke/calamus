#pragma once

#include <span>
#include <ui/structs.h>

namespace calamus {

class Texture final {
public:
    constexpr Texture() = default;
    explicit Texture(u32 gl_id, IntSize size);

    ~Texture();

    [[nodiscard]] constexpr auto id() const noexcept { return m_gl_id; }
    [[nodiscard]] constexpr auto size() const noexcept { return m_size; }
    [[nodiscard]] constexpr auto width() const noexcept { return m_size.width; }
    [[nodiscard]] constexpr auto height() const noexcept { return m_size.height; }

private:
    u32 m_gl_id {};
    IntSize m_size {};
};

}
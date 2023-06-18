#pragma once

#include <span>
#include <ui/structs.h>

namespace calamus {

/**
 * Textures can work in two ways:
 *  - single. One texture has its own ID and no parent information.
 *  - tiled.  There's one ID for the entire tilemap, any Texture instance uses that ID
 *            and contains its offset information into the parent tilemap texture.
 */
class Texture final {
public:
    constexpr Texture() = default;
    explicit Texture(
        u32 gl_id,
        IntSize size,
        IntSize parent_size = IntSize { 0, 0 },
        IntPosition offset_in_parent = IntPosition { 0, 0 }
    );

    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) noexcept = default;
    Texture& operator=(Texture&&) noexcept = default;

    [[nodiscard]] constexpr auto id() const noexcept { return m_gl_id; }
    [[nodiscard]] constexpr auto size() const noexcept { return m_size; }
    [[nodiscard]] constexpr auto width() const noexcept { return m_size.width; }
    [[nodiscard]] constexpr auto height() const noexcept { return m_size.height; }

    // Only relevant for textures derived from a tilemap
    [[nodiscard]] constexpr auto offset() const noexcept { return m_offset_in_parent; }
    [[nodiscard]] constexpr auto parent_size() const noexcept { return m_parent_size; }
    [[nodiscard]] constexpr auto parent_width() const noexcept { return m_parent_size.width; }
    [[nodiscard]] constexpr auto parent_height() const noexcept { return m_parent_size.height; }

private:
    u32 m_gl_id {};
    IntSize m_size {};

    IntSize m_parent_size {};
    IntPosition m_offset_in_parent {};
};

}
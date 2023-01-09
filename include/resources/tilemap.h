#pragma once

#include <raylib.h>
#include <util/types.h>

namespace calamus::Resources {

class Tilemap {
public:
    constexpr Tilemap() = default;
    constexpr Tilemap(Texture2D texture, i32 tile_size)
        : m_texture(texture)
        , m_width(texture.width)
        , m_height(texture.height)
        , m_tile_size(tile_size)
        , m_num_tiles_x(texture.width / tile_size)
        , m_num_tiles_y(texture.height / tile_size) {
    }

    ~Tilemap() { UnloadTexture(m_texture); }

    Tilemap(const Tilemap&) = default;
    Tilemap& operator=(const Tilemap&) = default;
    Tilemap(Tilemap&&) noexcept = default;
    Tilemap& operator=(Tilemap&&) noexcept = default;

    [[nodiscard]] constexpr Texture2D texture() const { return m_texture; }
    [[nodiscard]] constexpr i32 tile_size() const { return m_tile_size; }
    [[nodiscard]] constexpr i32 width() const { return m_num_tiles_x; }
    [[nodiscard]] constexpr i32 height() const { return m_num_tiles_y; }

private:
    Texture2D m_texture;

    i32 m_width;
    i32 m_height;
    i32 m_tile_size;
    i32 m_num_tiles_x;
    i32 m_num_tiles_y;
};

}

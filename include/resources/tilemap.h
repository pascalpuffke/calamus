#pragma once

#include <graphics/texture.h>
#include <util/common.h>

namespace calamus::Resources {

class Tilemap {
public:
    Tilemap() = default;
    Tilemap(const Texture& texture, i32 tile_size)
        : m_texture(texture)
        , m_tile_size(tile_size)
        , m_num_tiles_x(texture.width() / tile_size)
        , m_num_tiles_y(texture.height() / tile_size) {
    }

    Tilemap(const Tilemap&) = default;
    Tilemap& operator=(const Tilemap&) = default;
    Tilemap(Tilemap&&) noexcept = default;
    Tilemap& operator=(Tilemap&&) noexcept = default;

    [[nodiscard]] Texture texture() const { return m_texture; }
    [[nodiscard]] i32 tile_size() const { return m_tile_size; }
    [[nodiscard]] i32 width() const { return m_num_tiles_x; }
    [[nodiscard]] i32 height() const { return m_num_tiles_y; }

private:
    Texture m_texture;

    i32 m_tile_size {};
    i32 m_num_tiles_x {};
    i32 m_num_tiles_y {};
};

}

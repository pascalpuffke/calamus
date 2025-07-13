#include "assert.hpp"

#include <resources/level.h>

namespace calamus {

Level::Level(std::string&& name, u16 version, Size<u16> size, TextureMap&& texture_map, std::vector<TextureId>&& level_data)
    : m_name(std::move(name))
    , m_version(version)
    , m_size(size)
    , m_texture_map(std::move(texture_map))
    , m_level_data(std::move(level_data)) {
    VERIFY(m_level_data.size() == m_size.area());
}

}
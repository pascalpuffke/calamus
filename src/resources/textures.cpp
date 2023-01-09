#include <resources/textures.h>
#include <util/assert.h>

namespace calamus::Resources {

void TextureManager::load_tilemap(const std::filesystem::path& path, const char* name, i32 tile_size) {
    ASSERT_MSG(std::filesystem::exists(path), "File does not exist");
    auto texture = LoadTexture(absolute(path).c_str());
    auto tilemap = Tilemap(texture, tile_size);
    (*m_tilemaps)[name] = tilemap;
}

const Tilemap* TextureManager::tilemap(const char* name) {
    auto result = m_tilemaps->find(name);
    if (result == m_tilemaps->end())
        return nullptr;

    return &(std::get<Tilemap>(*result));
}

TextureManager::TextureManager() {
    m_tilemaps = std::make_unique<std::unordered_map<const char*, Tilemap>>();
}

}
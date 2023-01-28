#include <fstream>
#include <graphics/image.h>
#include <raylib.h>
#include <resources/textures.h>
#include <util/assert.h>
#include <util/reader.h>

namespace calamus::Resources {

Result<void> TextureManager::load_texture(TextureManager::TextureDescription&& texture_description) {
    return Result<void>::success();
}

Result<void> TextureManager::load_tilemap(TextureManager::TilemapDescription&& tilemap_description) {
    if (tilemap_description.path.extension() != ".png")
        return Error { "Expected .png extension" };

    // TODO This actually doesn't do anything yet

    return Result<void>::success();
}

TextureManager::TextureManager() {
    m_textures = std::make_unique<std::unordered_map<std::string, Texture>>();
}

const Texture& TextureManager::texture(std::string_view key) {
    // It feels kind of silly to be passed a string_view for "better performance", but then
    // still construct a fat string anyway because we can't index into the texture map.
    // Having m_textures key be string_view wouldn't work either, as texture names are defined
    // dynamically and we'll immediately run into invalid pointers. ?
    // I might also simply be stupid.
    auto result = m_textures->find(std::string { key });
    ASSERT_MSG(result != m_textures->end(), "tilemap not found");

    return result->second;
}

}
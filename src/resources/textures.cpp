#include <raylib.h>
#include <resources/textures.h>
#include <util/assert.h>

namespace calamus::Resources {

Result<void> TextureManager::load_texture(TextureManager::TextureDescription&& texture_description) {
    const auto& path = texture_description.path;
    const auto rl_texture = LoadTexture(path.c_str());
    auto texture = std::make_unique<calamus::Texture>(
        rl_texture.id,
        IntSize { rl_texture.width, rl_texture.height });

    auto [_, success] = m_textures.insert({ texture_description.name, std::move(texture) });
    if (!success)
        return Error::formatted("Couldn't insert texture {}", texture_description.name);

    LOG_DEBUG("Loaded texture '{}' with ID {}", texture_description.name, rl_texture.id);

    return Result<void>::success();
}

Result<void> TextureManager::load_tilemap(TextureManager::TilemapDescription&& tilemap_description) {
    if (tilemap_description.path.extension() != ".png")
        return Error { "Expected .png extension" };

    // TODO This actually doesn't do anything yet

    return Result<void>::success();
}

const Texture& TextureManager::texture(const std::string& key) {
    auto result = m_textures.find(key);
    ASSERT_MSG(result != m_textures.end(), "tilemap not found");

    // Lifetime of this texture is tied to this TextureManager instance, which practically means as long as
    // the application is running. Should be fine?
    return *(result->second);
}

TextureManager::TextureManager()
    : m_textures(std::unordered_map<std::string, std::unique_ptr<Texture>> {}) {
}

TextureManager::~TextureManager() = default;

}
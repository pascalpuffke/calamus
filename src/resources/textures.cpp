#include <raylib.h>
#include <resources/textures.h>
#include <util/assert.h>

namespace calamus::Resources {

Result<void> TextureManager::load_texture(TextureManager::TextureDescription&& texture_description) {
    const auto& path = texture_description.path;
    const auto& size = texture_description.size;
    const auto& name = texture_description.name;
    const auto rl_texture = LoadTexture(path.c_str());

    const auto actual_size = Size { rl_texture.width, rl_texture.height };
    if (actual_size != size) {
        UnloadTexture(rl_texture);
        return Error::formatted(
            "Specified size for texture '{}' does not match actual size (specified: {}, actual: {})",
            texture_description.name, size, actual_size);
    }

    auto texture = std::make_unique<calamus::Texture>(
        rl_texture.id,
        size);

    auto [_, success] = m_textures.insert({ name, std::move(texture) });
    if (!success)
        return Error::formatted("Couldn't insert texture '{}'", name);

    LOG_DEBUG("Loaded texture '{}' with ID {}", name, rl_texture.id);

    return Result<void>::success();
}

Result<void> TextureManager::load_tilemap(TextureManager::TilemapDescription&& tilemap_description) {
    const auto& path = tilemap_description.path;

    if (path.extension() != ".png")
        return Error { "Expected .png extension" };

    const auto tilemap_size = tilemap_description.tilemap_size;
    const auto tile_size = tilemap_description.tile_size;

    if ((tilemap_size.width % tile_size.width != 0) || (tilemap_size.height % tile_size.height != 0))
        return Error { "Tilemap size not divisible by size of a single tile" };

    const auto& tile_names = tilemap_description.tile_names;
    const auto rl_texture = LoadTexture(path.c_str());

    const auto tiles = tilemap_size / tile_size;
    const auto number_of_tiles = tile_names.size();
    for (auto x = 0; x < tiles.width; x++) {
        for (auto y = 0; y < tiles.height; y++) {
            const auto index = static_cast<size_t>(y * tiles.height + x);
            if (index >= number_of_tiles)
                break;

            const auto& name = tile_names[index];
            const auto offset_in_texture = IntPosition {
                x * tile_size.width,
                y * tile_size.height,
            };

            auto texture = std::make_unique<calamus::Texture>(
                rl_texture.id,
                tile_size,
                tilemap_size,
                offset_in_texture);
            auto [_, success] = m_textures.insert({ name, std::move(texture) });
            if (!success)
                return Error::formatted("Couldn't insert texture '{}'", name);

            LOG_DEBUG("Loaded tiled texture '{}' with parent ID {} at offset {}", name, rl_texture.id, offset_in_texture);
        }
    }

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
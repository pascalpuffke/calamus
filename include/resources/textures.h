#pragma once

#include <filesystem>
#include <graphics/texture.h>
#include <string>
#include <ui/structs.h>
#include <unordered_map>
#include <util/result.h>
#include <vector>

namespace calamus::Resources {

class TextureManager final {
public:
    TextureManager();
    ~TextureManager();

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    TextureManager(TextureManager&&) noexcept = delete;
    TextureManager& operator=(TextureManager&&) noexcept = delete;

    [[nodiscard]] auto load_texture(
        std::filesystem::path&& path,
        std::string&& name,
        IntSize size,
        TextureScaling scaling
    ) -> Result<void>;

    [[nodiscard]] auto load_tilemap(
        std::filesystem::path&& path,
        std::string&& tilemap_name,
        IntSize tilemap_size,
        IntSize tile_size,
        std::vector<std::string>&& tile_names
    ) -> Result<void>;

    [[nodiscard]] auto texture(const std::string&) -> const Texture&;

private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures {};

    // Avoid expensive map lookups when called in a loop by caching a pointer to the last requested texture
    // and return it when the currently asked for key equals the last one.
    Texture* m_last_texture_cache { nullptr };
    std::string m_last_key_cache;

    auto invalidate_texture_cache() -> void;
};

}

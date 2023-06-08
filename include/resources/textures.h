#pragma once

#include <filesystem>
#include <resources/tilemap.h>
#include <string>
#include <ui/structs.h>
#include <unordered_map>
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
        IntSize size
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
};

}

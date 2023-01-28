#pragma once

#include <filesystem>
#include <resources/tilemap.h>
#include <string>
#include <ui/structs.h>
#include <unordered_map>

namespace calamus::Resources {

class TextureManager final {
public:
    TextureManager();
    ~TextureManager() = default;

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    TextureManager(TextureManager&&) noexcept = delete;
    TextureManager& operator=(TextureManager&&) noexcept = delete;

    struct TextureDescription {
        std::filesystem::path path;
        std::string name;
        IntSize size;
    };
    [[nodiscard]] Result<void> load_texture(TextureDescription&&);

    struct TilemapDescription {
        std::filesystem::path path;
        std::string tilemap_name;
        IntSize tilemap_size;

        IntSize tile_size;
        std::vector<std::string> tile_names;
    };
    [[nodiscard]] Result<void> load_tilemap(TilemapDescription&&);

    [[nodiscard]] const Texture& texture(std::string_view);

private:
    std::unique_ptr<std::unordered_map<std::string, Texture>> m_textures {};
};

}

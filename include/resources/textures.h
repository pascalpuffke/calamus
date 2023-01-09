#pragma once

#include <filesystem>
#include <resources/tilemap.h>
#include <string>
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

    void load_tilemap(const std::filesystem::path& path, const char* name, i32 tile_size);
    [[nodiscard]] const Tilemap* tilemap(const char*);

private:
    std::unique_ptr<std::unordered_map<const char*, Tilemap>> m_tilemaps {};
};

}

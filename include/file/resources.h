#pragma once

#include <filesystem>
#include <fmt/std.h>
#include <resources/fonts.h>
#include <ui/structs.h>
#include <unordered_map>
#include <util/formatter.h>
#include <util/result.h>
#include <vector>

namespace calamus {

struct TextureResource {
    std::filesystem::path path;
    std::string name;
    IntSize size;
    bool tiled;
    IntSize tile_size;
    std::optional<std::vector<std::string>> tile_names;
};

class ResourceLoader final {
public:
    explicit ResourceLoader(std::filesystem::path root);
    ~ResourceLoader();

    [[nodiscard]] Result<std::vector<TextureResource>> find_textures();
    [[nodiscard]] Result<std::unordered_map<Resources::FontType, std::filesystem::path>> find_fonts();

private:
    std::filesystem::path m_root;
};

}

FORMATTER(calamus::TextureResource, "TextureResource(path={}, name='{}', size={}, tiled={}, tile_size={}, tile_names={})", value.path, value.name, value.size, value.tiled, value.tile_size, value.tile_names.has_value())

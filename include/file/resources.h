#pragma once

#include <filesystem>
#include <fmt/std.h>
#include <graphics/texture.h>
#include <resources/fonts.h>
#include <unordered_map>
#include <util/formatter.h>
#include <util/result.h>
#include <vector>

namespace calamus {

struct TextureResource {
    std::filesystem::path path;
    std::string name;
    IntSize size;
    Texture::Scaling scaling { Texture::Scaling::Stretch };

    bool tiled;
    IntSize tile_size;
    std::optional<std::vector<std::string>> tile_names;
};

struct FontResource {
    std::filesystem::path path;

    enum class Filter {
        Point,
        Bilinear,
        Trilinear,
        Anisotropic,
        _Error,
    } filter;
};

class ResourceLoader final {
public:
    explicit ResourceLoader(std::filesystem::path root);
    ~ResourceLoader();

    [[nodiscard]] auto find_textures() -> Result<std::vector<TextureResource>>;
    [[nodiscard]] auto find_fonts() -> Result<std::unordered_map<Resources::FontType, FontResource>>;

private:
    std::filesystem::path m_root;
};

}

FORMATTER(calamus::TextureResource, "TextureResource(path={}, name='{}', size={}, scaling={}, tiled={}, tile_size={}, tile_names={})", value.path, value.name, value.size, std::to_underlying(value.scaling), value.tiled, value.tile_size, value.tile_names.value_or(std::vector<std::string> {}))

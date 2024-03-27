#include <assert.hpp>
#include <file/resources.h>
#include <filesystem>
#include <fmt/std.h>
#include <raylib.h>
#include <util/raylib/raylib_wrapper.h>
#include <utility>

namespace calamus::Resources {

const Font& FontManager::get_font(FontType type) {
    const auto index = std::to_underlying(type);
    VERIFY(index < std::to_underlying(FontType::_Count));
    return m_fonts->at(type);
}

Result<void> FontManager::load_font(FontType type, const FontResource& resource) {
    if (m_fonts->contains(type))
        return Error::formatted("Another font is already registered for type {}", std::to_underlying(type));

    const auto& path = resource.path;

    if (!exists(path))
        return Error::formatted("File does not exist: {}", path);
    if (!is_regular_file(path))
        return Error::formatted("Not a regular file: {}", path);
    if (const auto extension = path.extension(); extension != ".ttf")
        return Error::formatted("Invalid file extension: {}", extension);

    auto font = wrapper::rtext::load_font(path);
    GenTextureMipmaps(&font.texture);
    switch (resource.filter) {
    case FontResource::Filter::Anisotropic:
        SetTextureFilter(font.texture, TEXTURE_FILTER_ANISOTROPIC_16X);
        break;
    case FontResource::Filter::Bilinear:
        SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
        break;
    case FontResource::Filter::Point:
        SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
        break;
    case FontResource::Filter::Trilinear:
        SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);
        break;
    default:
        std::unreachable();
    }

    auto [_, success] = m_fonts->insert({ type, font });
    if (!success)
        return Error::formatted("Couldn't insert font {}", path);

    return Result<void>::success();
}

FontManager::FontManager()
    : m_fonts(std::make_unique<std::unordered_map<FontType, Font>>()) {
}

FontManager::~FontManager() {
    for (const auto& [_, font] : *m_fonts) {
        wrapper::rtext::unload_font(font);
    }
}

}
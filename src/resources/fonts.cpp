#include <filesystem>
#include <fmt/std.h>
#include <raylib.h>
#include <resources/fonts.h>
#include <util/assert.h>
#include <util/raylib/raylib_extensions.h>
#include <utility>

namespace calamus::Resources {

const Font& FontManager::get_font(FontType type) {
    const auto index = std::to_underlying(type);
    ASSERT(index < std::to_underlying(FontType::_Count));
    return m_fonts->at(type);
}

Result<void> FontManager::load_font(FontType type, const std::filesystem::path& path) {
    if (m_fonts->contains(type))
        return Error::formatted("Another font is already registered for type {}", std::to_underlying(type));

    if (!exists(path))
        return Error::formatted("File does not exist: {}", path);
    if (!is_regular_file(path))
        return Error::formatted("Not a regular file: {}", path);
    if (const auto extension = path.extension(); extension != ".ttf")
        return Error::formatted("Invalid file extension: {}", extension);

    (*m_fonts)[type] = wrapper::rtext::load_font(path);

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
#include <filesystem>
#include <fmt/std.h>
#include <raylib.h>
#include <resources/fonts.h>
#include <unordered_map>
#include <util/assert.h>
#include <util/raylib/raylib_extensions.h>
#include <utility>

namespace calamus::Resources {

static std::unordered_map<FontType, Font> fonts {};

FontManagement& FontManagement::get() {
    static FontManagement instance;
    return instance;
}

const Font& FontManagement::get_font(FontType type) {
    const auto index = std::to_underlying(type);
    ASSERT(index < std::to_underlying(FontType::_Count));
    return fonts.at(type);
}

Result<void> FontManagement::load_font(FontType type, const std::filesystem::path& path) {
    if (fonts.contains(type))
        return Error::formatted("Another font is already registered for type {}", std::to_underlying(type));

    if (!exists(path))
        return Error::formatted("File does not exist: {}", path);
    if (!is_regular_file(path))
        return Error::formatted("Not a regular file: {}", path);
    if (const auto extension = path.extension(); extension != ".ttf")
        return Error::formatted("Invalid file extension: {}", extension);

    fonts[type] = wrapper::rtext::load_font(path);

    return Result<void>::success();
}

FontManagement::FontManagement() = default;

FontManagement::~FontManagement() {
    for (const auto& [_, font] : fonts) {
        wrapper::rtext::unload_font(font);
    }
}

}
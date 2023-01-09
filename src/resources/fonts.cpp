#include <filesystem>
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

Result<void> FontManagement::load_font(FontType type, std::string_view path) {
    if (fonts.contains(type))
        return Error::formatted("Another font is already registered for type {}", std::to_underlying(type));

    const auto fs_path = std::filesystem::path { path };
    if (!std::filesystem::exists(fs_path))
        return Error::formatted("File does not exist: '{}'", fs_path.string());
    if (!std::filesystem::is_regular_file(fs_path))
        return Error::formatted("Not a regular file: '{}'", fs_path.string());
    if (const auto extension = fs_path.extension(); extension != ".ttf")
        return Error::formatted("Invalid file extension: '{}'", extension.string());

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
#pragma once

#include <filesystem>
#include <unordered_map>
#include <util/result.h>

struct Font;

namespace calamus::Resources {

enum class FontType {
    Regular,
    Monospace,
    _Count,
};

class FontManager final {
public:
    FontManager();
    ~FontManager();

    FontManager(const FontManager&) = delete;
    FontManager(FontManager&&) noexcept = delete;
    FontManager& operator=(const FontManager&) = delete;
    FontManager& operator=(FontManager&&) noexcept = delete;

    const Font& get_font(FontType);
    Result<void> load_font(FontType, const std::filesystem::path&);

private:
    std::unique_ptr<std::unordered_map<FontType, Font>> m_fonts {};
};

}

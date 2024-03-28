#pragma once

#include <file/resources.h>
#include <filesystem>
#include <resources/enums.h>
#include <unordered_map>
#include <util/result.h>

struct Font;

namespace calamus {

namespace Resources {

    class FontManager final {
    public:
        FontManager();
        ~FontManager();

        FontManager(const FontManager&) = delete;
        FontManager(FontManager&&) noexcept = delete;
        FontManager& operator=(const FontManager&) = delete;
        FontManager& operator=(FontManager&&) noexcept = delete;

        [[nodiscard]] auto get_font(FontType) -> const Font&;
        [[nodiscard]] auto load_font(FontType, const FontResource&) -> Result<void>;

    private:
        std::unique_ptr<std::unordered_map<FontType, Font>> m_fonts {};
    };

}

}

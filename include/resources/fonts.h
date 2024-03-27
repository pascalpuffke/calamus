#pragma once

#include <filesystem>
#include <unordered_map>
#include <util/result.h>

struct Font;

namespace calamus {

// Forward decl to avoid circular include.
// TODO: I'm tired (file/resources.h depends on this header for Resources::FontType)
struct FontResource;

namespace Resources {

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

        [[nodiscard]] auto get_font(FontType) -> const Font&;
        [[nodiscard]] auto load_font(FontType, const FontResource&) -> Result<void>;

    private:
        std::unique_ptr<std::unordered_map<FontType, Font>> m_fonts {};
    };

}

}

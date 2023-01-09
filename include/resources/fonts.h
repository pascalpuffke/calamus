#pragma once

#include <util/result.h>

struct Font;

namespace calamus::Resources {

enum class FontType {
    Regular,
    Monospace,
    _Count,
};

class FontManagement final {
public:
    static FontManagement& get();

    FontManagement(const FontManagement&) = delete;
    FontManagement(FontManagement&&) noexcept = delete;

    FontManagement& operator=(const FontManagement&) = delete;
    FontManagement& operator=(FontManagement&&) noexcept = delete;

    const Font& get_font(FontType);
    Result<void> load_font(FontType, std::string_view);

private:
    FontManagement();
    ~FontManagement();
};

}

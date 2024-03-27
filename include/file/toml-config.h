#pragma once

#include <filesystem>
#include <util/result.h>
#include <util/types.h>

namespace calamus {

struct TomlConfig {
    i32 width;
    i32 height;
    i32 target_fps;
    i32 target_tps;
    bool vsync;
    bool limit_fps;
    bool debug;
    bool show_fps;
    bool draw_ui_bounds;
    bool raylib_log;
    std::string resources_root;
};

class TomlConfigLoader final {
public:
    [[nodiscard]] static auto load(const std::filesystem::path&) -> Result<TomlConfig>;
    [[nodiscard]] static auto load_or_default(const std::filesystem::path&) -> TomlConfig;
};

}

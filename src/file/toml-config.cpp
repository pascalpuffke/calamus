#include <file/toml-config.h>
#include <filesystem>
#include <fmt/color.h>
#include <fmt/std.h>
#include <toml++/toml.h>
#include <util/logging.h>

namespace fs = std::filesystem;

namespace calamus {

// std::string does not appear to be constexpr here.
// NOTE: Only tested against apple clang-1403.0.22.14.1 and libc++
#ifdef __clang__
#define MAYBE_CONSTEXPR_STD_STRING
#else
#define MAYBE_CONSTEXPR_STD_STRING constexpr
#endif

static MAYBE_CONSTEXPR_STD_STRING TomlConfig default_config = {
    .width = 800,
    .height = 600,
    .target_fps = 60,
    .target_tps = 20,
    .vsync = true,
    .limit_fps = true,
    .debug = false,
    .show_fps = false,
    .draw_ui_bounds = false,
    .raylib_log = false,
    .resources_root = "../res",
};

#undef MAYBE_CONSTEXPR_STD_STRING

Result<TomlConfig> TomlConfigLoader::load(const fs::path& path) {
    if (!path.has_extension() || path.extension() != ".toml")
        return Error { "Path must end with .toml" };

    if (!fs::exists(path))
        return Error { "TOML config file does not exist" };

    auto config = TomlConfig {};

    try {
        const auto config_toml = toml::parse_file(path.string());

#define TOML_CONFIG_LOAD_FIELD(parent, field) \
    config.field = config_toml[#parent][#field].value_or(default_config.field)
        TOML_CONFIG_LOAD_FIELD(window, width);
        TOML_CONFIG_LOAD_FIELD(window, height);
        TOML_CONFIG_LOAD_FIELD(window, vsync);
        TOML_CONFIG_LOAD_FIELD(window, limit_fps);
        TOML_CONFIG_LOAD_FIELD(window, target_fps);

        TOML_CONFIG_LOAD_FIELD(timing, target_tps);

        config.debug = config_toml["debug"]["enabled"].value_or(default_config.debug);
        TOML_CONFIG_LOAD_FIELD(debug, show_fps);
        TOML_CONFIG_LOAD_FIELD(debug, draw_ui_bounds);
        TOML_CONFIG_LOAD_FIELD(debug, raylib_log);

        TOML_CONFIG_LOAD_FIELD(resources, resources_root);
#undef TOML_CONFIG_LOAD_FIELD

        // Disable all debug features if debug is disabled
        if (!config.debug) {
            config.show_fps = false;
            config.raylib_log = false;
            config.draw_ui_bounds = false;
        }
    } catch (const toml::parse_error& e) {
        return Error { e.what() };
    }

    LOG_INFO("Loaded config from path: {}", fs::absolute(path))
    return config;
}

TomlConfig TomlConfigLoader::load_or_default(const fs::path& path) {
    auto config = load(path);
    return config.value_or_else([&]() {
        LOG_WARNING("no TOML config found, using default: {}", config.error());
        return default_config;
    });
}

}

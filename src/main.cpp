#include <file/resources.h>
#include <graphics/renderer.h>
#include <raylib.h>
#include <resources/fonts.h>
#include <resources/textures.h>
#include <ui/layout/fixed_layout.h>
#include <ui/layout/grid_layout.h>
#include <ui/ui_button.h>
#include <ui/ui_screen.h>
#include <util/common.h>
#include <util/raylib/raylib_wrapper.h>

static calamus::State global_state;
calamus::State& state = global_state;

namespace calamus {

constexpr static auto toml_path = std::string_view { "../config.toml" };

void raylib_log_callback(i32 level, const char* text, va_list args) {
    if (!state.config->raylib_log)
        return;
    if (level == LOG_TRACE)
        return;

    // fmtlib does not support va_list for safety reasons, so let's be unsafe ourselves!
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), text, args);

    switch (level) {
    case LOG_DEBUG:
        LOG_DEBUG("{}", buffer)
        break;
    case LOG_INFO:
        LOG_INFO("{}", buffer)
        break;
    case LOG_WARNING:
        LOG_WARNING("{}", buffer)
        break;
    case LOG_ERROR:
        LOG_ERROR("{}", buffer)
        break;
    case LOG_FATAL:
        LOG_ERROR("{}", buffer)
        LOG_ERROR("fatal error, terminating...")
        std::terminate();
    default:
        break;
    }
}

void register_screens() {
    const auto margin = 30;
    const auto font_size = 28;
    auto* window = VERIFY_PTR(state.window);
    auto* manager = VERIFY_PTR(state.screen_manager);

    manager->register_screen(Screen::Game, UI::ScreenLayout::create<UI::FixedLayout>({}));
    auto editor_layout = std::make_unique<UI::GridLayout>();
    editor_layout->set_columns_homogeneous(true);
    editor_layout->set_rows_homogeneous(true);
    editor_layout->set_column_count(3);
    editor_layout->set_row_count(2);
    editor_layout->set_column_spacing(10);
    editor_layout->set_row_spacing(10);

    // clang-format off
    manager->register_screen(Screen::Editor,
        UI::ScreenLayout::create(std::move(editor_layout), {
            UI::Button::create(UI::Label::create("1", font_size), {}, {}, [&](auto) { LOG_DEBUG("1") }),
            UI::Button::create(UI::Label::create("2", font_size), {}, {}, [&](auto) { LOG_DEBUG("2") }),
            UI::Button::create(UI::Label::create("3", font_size), {}, {}, [&](auto) { LOG_DEBUG("3") }),
            UI::Button::create(UI::Label::create("4", font_size), {}, {}, [&](auto) { LOG_DEBUG("4") }),
            UI::Button::create(UI::Label::create("5", font_size), {}, {}, [&](auto) { LOG_DEBUG("5") }),
            UI::Button::create(UI::Label::create("6", font_size), {}, {}, [&](auto) { LOG_DEBUG("6") }),
        }));

    manager->register_screen(Screen::Menu,
        UI::ScreenLayout::create<UI::FixedLayout>({
            UI::Button::create(
                UI::Label::create("Start", font_size),
                Position { margin, margin },
                Size {
                    window->size().width - 2 * margin,
                    window->size().height / 5,
                },
                [&](auto) {
                    state.current_screen = Screen::Game;
                }),
            UI::Button::create(
                UI::Label::create("Editor", font_size),
                Position { margin, (window->size().height / 5) + 2 * margin },
                Size {
                    window->size().width - 2 * margin,
                    window->size().height / 5,
                },
                [&](auto) {
                    state.current_screen = Screen::Editor;
                }),
            UI::Button::create(
                UI::Label::create("Settings", font_size),
                Position { margin, 2 * (window->size().height / 5) + 3 * margin },
                Size {
                    (window->size().width - 2 * margin) / 2,
                    window->size().height / 5,
                },
                [&](auto) {
                    state.current_screen = Screen::Settings;
                }),
            UI::Button::create(
                UI::Label::create("Quit", font_size),
                Position { margin + (window->size().width / 2), 2 * (window->size().height / 5) + 3 * margin },
                Size {
                    (window->size().width - 120) / 2,
                    window->size().height / 5,
                },
                [&](auto) {
                    window->close();
                    std::exit(0);
                }),
        }));
    // clang-format on
}

Result<void> load_resources() {
    auto* texture_manager = VERIFY_PTR(state.texture_manager);
    auto* font_manager = VERIFY_PTR(state.font_manager);

    const auto load_tilemap = [=](TextureResource& resource) -> Result<void> {
        if (!resource.tile_names)
            return Error { "Can't load tilemap with empty tile_names vector" };

        TRY(texture_manager->load_tilemap(
            std::move(resource.path),
            std::move(resource.name),
            resource.size,
            resource.tile_size,
            std::move(*resource.tile_names)
        ));

        return Result<void>::success();
    };

    const auto load_texture = [=](TextureResource& resource) -> Result<void> {
        TRY(texture_manager->load_texture(
            std::move(resource.path),
            std::move(resource.name),
            resource.size
        ));

        return Result<void>::success();
    };

    const auto loader = std::make_unique<ResourceLoader>(state.config->resources_root);
    auto resources = TRY(loader->find_textures());
    for (auto& resource : resources) {
        if (resource.tiled)
            TRY(load_tilemap(resource));
        else
            TRY(load_texture(resource));
    }

    auto fonts = TRY(loader->find_fonts());
    for (const auto& [type, path] : fonts) {
        TRY(font_manager->load_font(type, path));
    }

    return Result<void>::success();
}

}

int main() {
    using namespace calamus;

    // The exact order is immensely important.
    // Some components, such as ScreenManager, depend on a renderer being instantiated *before*, for example to register callbacks.
    auto config = std::make_unique<TomlConfig>(TomlConfigLoader::load_or_default(toml_path));
    state.config = config.get();
    auto textures = std::make_unique<Resources::TextureManager>();
    state.texture_manager = textures.get();
    auto fonts = std::make_unique<Resources::FontManager>();
    state.font_manager = fonts.get();
    auto window = std::make_unique<Window>();
    state.window = window.get();
    auto renderer = std::make_unique<Renderer>();
    state.renderer = renderer.get();
    auto screens = std::make_unique<UI::ScreenManager>();
    state.screen_manager = screens.get();

    // Has to be the first raylib call, otherwise it will not use a custom log callback
    SetTraceLogCallback(raylib_log_callback);

    window->init();
    window->set_title("dingus");
    const auto get_unix_second = [&]() {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        );
    };
    auto previous_frame_count = u64 { 0 };
    auto previous_time = get_unix_second();
    renderer->install_prerender_callback([&](auto frame_count) {
        auto time = get_unix_second();
        if (previous_time != time) {
            auto frames_since_last = frame_count - previous_frame_count;
            LOG_DEBUG("frame {} ({} fps)", frame_count, frames_since_last);
            previous_frame_count = frame_count;
        }
        previous_time = time;
    });
    renderer->attach(window.get());

    // Raylib supports a maximum of 4 gamepads.
    for (auto i = 0; i < 4; i++) {
        if (!wrapper::rcore::is_gamepad_available(i))
            continue;

        auto name = wrapper::rcore::get_gamepad_name(i);
        auto axis = wrapper::rcore::get_gamepad_axis_count(i);
        LOG_INFO("Found gamepad {}: '{}', {} axis", i, name, axis);
    }

    if (auto resources_result = load_resources(); !resources_result) {
        LOG_ERROR("Error loading resources: {}", resources_result.error())
        LOG_ERROR("Proceeding anyway, horrible things might happen.")
    }

    register_screens();
    renderer->start();

    return 0;
}

#include <file/resources.h>
#include <graphics/renderer.h>
#include <raylib.h>
#include <resources/fonts.h>
#include <resources/textures.h>
#include <ui/layout/fixed_layout.h>
#include <ui/ui_button.h>
#include <ui/ui_screen.h>
#include <util/common.h>

static calamus::State global_state;
calamus::State& state = global_state;

namespace calamus {

constexpr static auto toml_path = std::string_view { "../config.toml" };

// Exclusively for raylib logging
void log_callback(i32 level, const char* text, va_list args) {
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
    manager->register_screen(Screen::Editor,
        UI::ScreenLayout::create<UI::FixedLayout>({
            UI::Button::create(
                UI::Label::create("Center!", font_size),
                Position { margin, margin },
                Size {
                    window->size().width - 2 * margin,
                    font_size * 2,
                },
                [&](auto pos) {
                    LOG_DEBUG("Clicked button 1 at {}", pos)
                },
                UI::Alignment::Center),
            UI::Button::create(
                UI::Label::create("Center left!", font_size),
                Position { margin, (static_cast<i32>(margin * 1.5f) + font_size * 2) },
                Size {
                    window->size().width - 2 * margin,
                    font_size * 2,
                },
                [&](auto pos) {
                    LOG_DEBUG("Clicked button 2 at {}", pos)
                },
                UI::Alignment::CenterLeft),
            UI::Button::create(
                UI::Label::create("Center right!", font_size),
                Position { margin, (static_cast<i32>(margin * 2) + font_size * 4) },
                Size {
                    window->size().width - 2 * margin,
                    font_size * 2,
                },
                [&](auto pos) {
                    LOG_DEBUG("Clicked button 3 at {}", pos)
                },
                UI::Alignment::CenterRight),
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
}

Result<void> load_resources() {
    auto* texture_manager = VERIFY_PTR(state.texture_manager);
    auto* font_manager = VERIFY_PTR(state.font_manager);

    const auto loader = std::make_unique<ResourceLoader>(state.config->resources_root);

    auto textures = TRY(loader->find_textures());
    for (auto& texture_resource : textures) {
        // TODO Tiled textures
        auto description = Resources::TextureManager::TextureDescription {
            std::move(texture_resource.path),
            std::move(texture_resource.name),
            texture_resource.size,
        };
        TRY(texture_manager->load_texture(std::move(description)));
    }

    auto fonts = TRY(loader->find_fonts());
    font_manager->load_font(Resources::FontType::Monospace, fonts[Resources::FontType::Monospace]);
    font_manager->load_font(Resources::FontType::Regular, fonts[Resources::FontType::Regular]);

    return Result<void>::success();
}

}

int main() {
    using namespace calamus;

    auto config = std::make_unique<TomlConfig>(TomlConfigLoader::load_or_default(toml_path));
    state.config = config.get();
    auto textures = std::make_unique<Resources::TextureManager>();
    state.texture_manager = textures.get();
    auto fonts = std::make_unique<Resources::FontManager>();
    state.font_manager = fonts.get();
    auto screens = std::make_unique<UI::ScreenManager>();
    state.screen_manager = screens.get();
    auto window = std::make_unique<Window>();
    state.window = window.get();
    auto renderer = std::make_unique<Renderer>();
    state.renderer = renderer.get();

    // Has to be the first raylib call, otherwise it will not use a custom log callback
    SetTraceLogCallback(log_callback);

    window->init();
    window->set_title("dingus");
    renderer->attach(window.get());

    if (auto resources_result = load_resources(); resources_result.has_error()) {
        LOG_ERROR("Error loading resources: {}", resources_result.error())
        LOG_ERROR("Proceeding anyway, horrible things might happen.")
    }

    register_screens();
    renderer->start();

    return 0;
}

#pragma once

struct Font;

namespace calamus {

struct TomlConfig;

class Renderer;
class Window;

namespace UI {
    class ScreenManager;
}

namespace Resources {
    class TextureManager;
    class FontManager;
}

enum class Screen {
    Menu,
    Game,
    Editor,
    Settings,
};

struct State {
    // Game configuration as initialized from a config.toml file.
    TomlConfig* config { nullptr };

    // Singletons. (Some of these probably shouldn't be.)
    Renderer* renderer { nullptr };
    Window* window { nullptr };
    UI::ScreenManager* screen_manager { nullptr };
    Resources::TextureManager* texture_manager { nullptr };
    Resources::FontManager* font_manager { nullptr };

    Screen current_screen = Screen::Menu;
};

}

// Defined in main
extern calamus::State& state;

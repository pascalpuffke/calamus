#include <graphics/renderer.h>
#include <resources/textures.h>
#include <ui/ui_button.h>
#include <ui/ui_label.h>
#include <ui/ui_screen.h>
#include <util/logging.h>
#include <util/raylib/raylib_wrapper.h>

namespace calamus {

using namespace wrapper;

class UILayer final : public RenderLayer {
public:
    explicit UILayer(Renderer& renderer)
        : RenderLayer(renderer) { }

    void on_render() override {
        const auto& layout = VERIFY_PTR(state.screen_manager)->layout(state.current_screen);
        for (const auto& object : layout.children())
            object->draw();
    }
};

class DebugLayer final : public RenderLayer {
public:
    DebugLayer(Renderer& renderer, bool draw_fps, bool draw_ui_bounds)
        : RenderLayer(renderer)
        , m_draw_fps(draw_fps)
        , m_draw_ui_bounds(draw_ui_bounds) { }

    void on_render() override {
        if (m_draw_fps)
            draw_fps(Position { 10, 10 }, 20, default_palette::green);

        if (m_draw_ui_bounds) {
            const auto& layout = VERIFY_PTR(state.screen_manager)->layout(state.current_screen);
            for (const auto& object : layout.children())
                draw_ui_bounds(object);
        }
    }

private:
    bool m_draw_fps { false };
    bool m_draw_ui_bounds { false };

    void draw_ui_bounds(const std::shared_ptr<UI::Object>& object) {
        const auto& position = object->position();
        const auto& size = object->size();
        const auto bounds = IntRect {
            position.x - 1, position.y - 1,
            size.width + 2, size.height + 2
        };
        rshapes::draw_rectangle_outline(bounds, 1.0f, default_palette::red);

        if (object->type() == UI::ObjectType::Button) {
            const auto& button = object->as_ref<UI::Button>();
            const auto& inner_label = button.label();
            draw_ui_bounds(inner_label);
        }
    }

    void draw_fps(IntPosition position, i32 font_size, Color color) {
        const auto frametime = rcore::get_frame_time();
        const auto fps = rcore::get_fps();
        const auto fps_string = fmt::format("{} fps ({:.02f}ms/f) {}", fps, frametime, renderer.frame_count());
        rtext::draw_text(fps_string, position, font_size, color, Resources::FontType::Monospace);

        const auto pos = renderer.window().position();
        const auto pos_string = fmt::format("{}", pos);
        rtext::draw_text(pos_string, IntPosition { position.x, position.y + font_size }, font_size, color, Resources::FontType::Monospace);

        const auto size = renderer.window().size();
        const auto size_string = fmt::format("{}", size);
        rtext::draw_text(size_string, IntPosition { position.x, position.y + (font_size * 2) }, font_size, color, Resources::FontType::Monospace);
    }
};

class WorldTestLayer final : public RenderLayer {
public:
    explicit WorldTestLayer(Renderer& renderer)
        : RenderLayer(renderer) { }

    void on_render() override {
        static constexpr auto world = std::array {
            std::array { 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g' },
            std::array { 'g', 'g', 'g', 'd', 'd', 'g', 'g', 'g' },
            std::array { 'g', 'g', 'd', 'd', 'd', 'd', 'g', 'g' },
            std::array { 'g', 'd', 'd', 'd', 'd', 'd', 'd', 'g' },
            std::array { 'g', 'd', 'd', 'd', 'd', 'd', 'd', 'g' },
            std::array { 'g', 'g', 'd', 'd', 'd', 'd', 'g', 'g' },
            std::array { 'g', 'g', 'g', 'd', 'd', 'g', 'g', 'g' },
            std::array { 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g' },
        };

        auto texture_map = std::unordered_map<char, std::string> {
            { 'g', "grass" },
            { 'd', "dirt" },
        };

        const auto win_size = renderer.window().size();
        for (auto x = 0; x < win_size.width; x += 16) {
            for (auto y = 0; y < win_size.height; y += 16) {
                const auto index_x = std::min(usize(x / 16), world.size() - 1);
                const auto index_y = std::min(usize(y / 16), world[0].size() - 1);
                const auto tile_char = world[index_x][index_y];
                const auto texture_key = texture_map[tile_char];
                const auto& texture = VERIFY_PTR(state.texture_manager)->texture(texture_key);
                renderer.draw_texture(texture, IntPosition { x, y });
            }
        }
    }
};

class CursorLayer final : public RenderLayer {
public:
    explicit CursorLayer(Renderer& renderer)
        : RenderLayer(renderer) { }

    void on_render() override {
        const auto absolute_position = rcore::get_mouse_position();
        auto* textures = VERIFY_PTR(state.texture_manager);
        const auto& cursor = textures->texture("cursor");

        renderer.draw_texture(cursor, absolute_position);
    }
};

Renderer::Renderer() = default;

Renderer::~Renderer() = default;

void Renderer::attach(Window* window) {
    m_window = window;
    m_window->refresh();
    m_window->install_resize_callback([](auto new_size) {
        LOG_DEBUG("resized: {}", new_size);

        auto& screen = state.screen_manager->layout(state.current_screen);
        screen.rebuild_layout(new_size);
    });
    m_window->install_move_callback([](auto new_position) {
        LOG_DEBUG("moved: {}", new_position);
    });
}

void Renderer::prepare() {
    rcore::begin_drawing();
    rcore::clear_background(default_palette::black);
    rcore::begin_mode_2d(m_camera);

    // TODO: Add input management and move these out of Renderer.
    if (rcore::is_key_pressed(Key::W))
        state.current_screen = Screen::Game;
    if (rcore::is_key_pressed(Key::E))
        state.current_screen = Screen::Menu;
}

void Renderer::notify_prerender_callbacks() {
    for (const auto& callback : m_prerender_callbacks) {
        callback(frame_count());
    }
}

void Renderer::start() {
    VERIFY_PTR(m_window);

    auto* config = VERIFY_PTR(state.config);

    install_layer<WorldTestLayer>(LayerSpace::WorldSpace);
    install_layer<UILayer>(LayerSpace::ScreenSpace);
    if (config->debug)
        install_layer<DebugLayer>(LayerSpace::ScreenSpace, config->show_fps, config->draw_ui_bounds);
    install_layer<CursorLayer>(LayerSpace::ScreenSpace);

    while (!m_window->should_close()) {
        notify_prerender_callbacks();

        prepare();
        render_world_space_layers();

        // To prevent the camera from affecting UI elements, we need to end 2d mode here.
        // It would be more elegant to move this call to 'finalize()'
        rcore::end_mode_2d();
        render_screen_space_layers();

        finalize();
    }
}

void Renderer::finalize() {
    rcore::end_drawing();

    m_window->refresh();
    ++m_frame_count;
}

void Renderer::render_world_space_layers() {
    for (const auto& layer : m_world_space_layers)
        layer->on_render();
}

void Renderer::render_screen_space_layers() {
    for (const auto& layer : m_screen_space_layers)
        layer->on_render();
}

void Renderer::draw_texture(const Texture& texture, IntPosition position) {
    if (position > m_window->size().to_position())
        return;

    wrapper::rtextures::draw_texture(texture, position, texture.size());
}

void Renderer::install_prerender_callback(const render_callback& callback) {
    m_prerender_callbacks.emplace_back(callback);
}

template <typename Layer, typename... Args>
    requires std::is_base_of_v<RenderLayer, Layer>
void Renderer::install_layer(LayerSpace space, Args&&... args) {
    auto layer = std::make_unique<Layer>(*this, std::forward<Args>(args)...);
    if (space == LayerSpace::ScreenSpace)
        m_screen_space_layers.emplace_back(std::move(layer));
    else
        m_world_space_layers.emplace_back(std::move(layer));

    LOG_INFO("Installed layer '{}' to {}", type_name<Layer>(), space == LayerSpace::ScreenSpace ? "screen space" : "world space");
}

}

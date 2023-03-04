#include <functional>
#include <graphics/renderer.h>
#include <resources/textures.h>
#include <ui/ui_button.h>
#include <ui/ui_label.h>
#include <ui/ui_screen.h>
#include <util/logging.h>
#include <util/raylib/raylib_wrapper.h>
#include <utility>

namespace calamus {

using namespace wrapper;

class RenderingScope {
public:
    USED explicit RenderingScope(std::function<void()> render, std::function<void()> ui)
        : render_function(std::move(render))
        , ui_function(std::move(ui)) {
        rcore::begin_drawing();
        rcore::clear_background(default_palette::black);
        render_function();
    }

    ~RenderingScope() {
        ui_function();
        rcore::end_drawing();
    }

private:
    std::function<void()> render_function;
    std::function<void()> ui_function;
};

Renderer::Renderer() = default;

void Renderer::attach(Window* window) {
    m_window = window;
    m_window->refresh();
    m_window->install_resize_callback([](auto new_size) {
        LOG_DEBUG("resized: {}", new_size);
    });
    m_window->install_move_callback([](auto new_position) {
        LOG_DEBUG("moved: {}", new_position);
    });
}

void Renderer::start() {
    VERIFY_PTR(m_window);

    while (!m_window->should_close()) {
        RenderingScope scope {
            [this]() {
                // TODO this should absolutely NOT be inside the renderer.
                if (rcore::is_key_pressed(Key::W))
                    state.current_screen = Screen::Game;
                if (rcore::is_key_pressed(Key::E))
                    state.current_screen = Screen::Menu;
                if (rcore::is_key_pressed(Key::L))
                    LOG_DEBUG("{}", m_window->properties());
                if (rcore::is_key_down(Key::R))
                    m_window->set_position(m_window->position() + IntPosition { 10 });
                if (rcore::is_key_down(Key::C)) {
                    m_camera.set_offset(m_camera.offset() + IntPosition { 10 });
                    println(m_camera.offset());
                }

                const auto mouse_position = rcore::get_mouse_position();
                state.screen_manager->check_hover(mouse_position);

                for (auto button_index = 0; button_index < std::to_underlying(MouseButton::__Count); button_index++) {
                    const auto button = static_cast<MouseButton>(button_index);
                    if (rcore::is_mouse_button_pressed(button))
                        state.screen_manager->check_click(button, mouse_position);
                }

                render();
            },
            [this]() {
                draw_ui();
                m_window->refresh();
                ++m_frame;
            } };
    }
}

void Renderer::render() {
    const auto& grass = state.texture_manager->texture("grass");
    const auto win_size = m_window->size();

    for (auto x = 0; x < win_size.width; x += grass.width()) {
        for (auto y = 0; y < win_size.height; y += grass.height()) {
            draw_texture(grass, IntPosition { x, y });
        }
    }
}

void Renderer::draw_texture(const Texture& texture, IntPosition position) {
    if (position > m_window->size().to_position())
        return;

    wrapper::rtextures::draw_texture(texture, position, texture.size());
}

void Renderer::draw_ui() {
    const auto& layout = state.screen_manager->layout(state.current_screen);
    for (const auto& object : layout.children()) {
        object->draw();
        if (state.config->draw_ui_bounds)
            draw_ui_bounds(object);
    }

    if (state.config->show_fps)
        draw_fps(Position { 10, 10 }, 20, default_palette::green);
}

void Renderer::draw_ui_bounds(const std::shared_ptr<UI::Object>& object) {
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

void Renderer::draw_fps(IntPosition position, i32 font_size, Color color) {
    const auto frametime = rcore::get_frame_time();
    const auto fps = rcore::get_fps();
    const auto fps_string = fmt::format("{} fps ({:.02f}ms/f) {}", fps, frametime, m_frame);
    rtext::draw_text(fps_string, position, font_size, color, Resources::FontType::Monospace);

    const auto pos = m_window->position();
    const auto pos_string = fmt::format("{}", pos);
    rtext::draw_text(pos_string, IntPosition { position.x, position.y + font_size }, font_size, color, Resources::FontType::Monospace);

    const auto size = m_window->size();
    const auto size_string = fmt::format("{}", size);
    rtext::draw_text(size_string, IntPosition { position.x, position.y + (font_size * 2) }, font_size, color, Resources::FontType::Monospace);
}

}

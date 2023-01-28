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
    USED explicit RenderingScope(const Camera& camera, std::function<void()> render, std::function<void()> ui)
        : render_function(std::move(render))
        , ui_function(std::move(ui)) {
        // LOG_DEBUG("camera {} offset={} target={} rotation={} zoom={}", fmt::ptr(camera), camera->offset, camera->target, camera->rotation, camera->zoom);
        rcore::begin_drawing();
        rcore::clear_background(default_palette::black);
        rcore::begin_mode_2d(camera);
        render_function();
    }

    ~RenderingScope() {
        rcore::end_mode_2d();
        ui_function();
        rcore::end_drawing();
    }

private:
    std::function<void()> render_function;
    std::function<void()> ui_function;
};

Renderer::Renderer() = default;

static void on_resize(IntSize size) {
    LOG_DEBUG("resized: {}", size);
}

static void on_move(IntPosition position) {
    LOG_DEBUG("moved: {}", position);
}

void Renderer::attach(Window* window) {
    m_window = window;
    m_window->refresh();
    m_window->install_resize_callback(on_resize);
    m_window->install_move_callback(on_move);
    m_camera.set_target(IntPosition {
        m_window->size().width / 2,
        m_window->size().height / 2 });
}

void Renderer::start() {
    VERIFY_PTR(m_window);

    while (!m_window->should_close()) {
        RenderingScope scope { m_camera,
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
                if (rcore::is_mouse_button_pressed(MouseButton::Left))
                    state.screen_manager->check_click(MouseButton::Left, mouse_position);
                if (rcore::is_mouse_button_pressed(MouseButton::Right))
                    state.screen_manager->check_click(MouseButton::Right, mouse_position);
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
}

void Renderer::draw_ui() {
    const auto& layout = state.screen_manager->layout(state.current_screen);
    for (const auto& object : layout.children()) {
        object->draw();
        if (state.config->draw_ui_bounds)
            draw_ui_bounds(object.get());
    }

    if (state.config->show_fps)
        draw_fps(Position { 10, 10 }, 20, default_palette::green);
}

void Renderer::draw_ui_bounds(UI::Object* object) {
    const auto& position = object->position();
    const auto& size = object->size();
    const auto bounds = IntRect {
        position.x - 1, position.y - 1,
        size.width + 2, size.height + 2
    };
    rshapes::draw_rectangle_outline(bounds, 1.0f, default_palette::red);

    if (object->type() == UI::ObjectType::Button) {
        const auto* button = dynamic_cast<UI::Button*>(object); // Yes, I use dynamic_cast. Sue me, go ahead
        const auto& inner_label = button->label();
        draw_ui_bounds(inner_label.get());
    }
}

void Renderer::draw_fps(IntPosition position, i32 font_size, Color color) {
    const auto frametime = rcore::get_frame_time();
    const auto fps = rcore::get_fps();
    const auto string = fmt::format("{} fps ({:.02f}ms/f) {}", fps, frametime, m_frame);
    rtext::draw_text(string, position, font_size, color, Resources::FontType::Monospace);

    const auto pos = m_window->position();
    const auto pos_string = fmt::format("{} {}", pos.x, pos.y);
    rtext::draw_text(pos_string, IntPosition { position.x, position.y + font_size }, font_size, color, Resources::FontType::Monospace);

    const auto size = m_window->size();
    const auto size_string = fmt::format("{}x{}", size.width, size.height);
    rtext::draw_text(size_string, IntPosition { position.x, position.y + (font_size * 2) }, font_size, color, Resources::FontType::Monospace);
}

}

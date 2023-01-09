#include <functional>
#include <graphics/renderer.h>
#include <raylib.h>
#include <resources/textures.h>
#include <ui/ui_button.h>
#include <ui/ui_label.h>
#include <ui/ui_screen.h>
#include <util/logging.h>
#include <util/raylib/raylib_extensions.h>
#include <utility>

namespace calamus {

class RenderingScope {
public:
    USED explicit RenderingScope(Camera2D* camera, std::function<void()> render, std::function<void()> ui)
        : render_function(std::move(render))
        , ui_function(std::move(ui)) {
        // LOG_DEBUG("camera {} offset={} target={} rotation={} zoom={}", fmt::ptr(camera), camera->offset, camera->target, camera->rotation, camera->zoom);
        wrapper::rcore::begin_drawing();
        wrapper::rcore::clear_background(default_palette::black);
        wrapper::rcore::begin_mode_2d(*camera);
        render_function();
    }

    ~RenderingScope() {
        wrapper::rcore::end_mode_2d();
        ui_function();
        wrapper::rcore::end_drawing();
    }

private:
    std::function<void()> render_function;
    std::function<void()> ui_function;
};

Renderer::Renderer() {
    m_camera = std::make_unique<Camera2D>();
    m_camera->zoom = 1.0f;
    m_camera->rotation = 0.0f;
    m_camera->target = Vector2 { 0.0f, 0.0f };
    m_camera->offset = Vector2 { 0.0f, 0.0f };
}

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
    m_camera->target = rl_vec_from(Position { m_window->size().width / 2, m_window->size().height / 2 });
}

void Renderer::start() {
    VERIFY_PTR(m_window);
    VERIFY_PTR(m_camera.get());

    while (!m_window->should_close()) {
        RenderingScope scope { m_camera.get(),
            [this]() {
                // TODO this should absolutely NOT be inside the graphics.
                if (wrapper::rcore::is_key_pressed(KEY_W))
                    state.current_screen = Screen::Game;
                if (wrapper::rcore::is_key_pressed(KEY_E))
                    state.current_screen = Screen::Menu;
                if (wrapper::rcore::is_key_pressed(KEY_L))
                    LOG_DEBUG("{}", m_window->properties());
                if (wrapper::rcore::is_key_down(KEY_R))
                    m_window->set_position(m_window->position() + Position { 10 });
                const auto mouse_position = wrapper::rcore::get_mouse_position();
                state.screen_manager->check_hover(mouse_position);
                if (wrapper::rcore::is_mouse_button_pressed(MouseButton::Left))
                    state.screen_manager->check_click(MouseButton::Left, mouse_position);
                if (wrapper::rcore::is_mouse_button_pressed(MouseButton::Right))
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
    for (const auto& object : layout.objects) {
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
    wrapper::rshapes::draw_rectangle_outline(bounds, 1.0f, default_palette::red);

    if (object->type() == UI::ObjectType::Button) {
        const auto* button = dynamic_cast<UI::Button*>(object); // Yes, I use dynamic_cast. Sue me, go ahead
        const auto& inner_label = button->label();
        draw_ui_bounds(inner_label.get());
    }
}

void Renderer::draw_fps(IntPosition position, i32 font_size, Color color) {
    const auto frametime = wrapper::rcore::get_frame_time();
    const auto fps = wrapper::rcore::get_fps();
    const auto string = fmt::format("{} fps ({:.02f}ms/f) {}", fps, frametime, m_frame);
    wrapper::rtext::draw_text(string, position, font_size, color, Resources::FontType::Monospace);

    const auto pos = m_window->position();
    const auto pos_string = fmt::format("{} {}", pos.x, pos.y);
    wrapper::rtext::draw_text(pos_string, IntPosition { position.x, position.y + font_size }, font_size, color, Resources::FontType::Monospace);

    const auto size = m_window->size();
    const auto size_string = fmt::format("{}x{}", size.width, size.height);
    wrapper::rtext::draw_text(size_string, IntPosition { position.x, position.y + (font_size * 2) }, font_size, color, Resources::FontType::Monospace);
}

}

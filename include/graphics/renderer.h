#pragma once

#include <graphics/camera.h>
#include <graphics/texture.h>
#include <graphics/window.h>

namespace calamus {

namespace UI {
    class Object;
}

class Renderer final {
public:
    friend class RenderingScope;

    Renderer();
    ~Renderer() = default;

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) noexcept = delete;
    Renderer& operator=(Renderer&&) noexcept = delete;

    void attach(Window*);
    void start();

    using render_callback = std::function<void(u64)>;
    void install_prerender_callback(const render_callback&);

    [[nodiscard]] auto frame_count() const noexcept { return m_frame_count; }

private:
    void render();

    void draw_texture(const calamus::Texture&, IntPosition);
    void draw_ui();
    void draw_ui_bounds(const std::shared_ptr<UI::Object>&);
    void draw_fps(IntPosition, i32, Color);

    Window* m_window { nullptr };
    Texture m_render_texture {};
    Camera m_camera {};
    u64 m_frame_count { 0 };

    std::vector<render_callback> m_prerender_callbacks {};
};

}

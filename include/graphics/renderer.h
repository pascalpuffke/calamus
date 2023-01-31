#pragma once

#include <graphics/camera.h>
#include <graphics/window.h>

namespace calamus {

namespace UI {
    class Object;
}

class Renderer final {
public:
    Renderer();
    ~Renderer() = default;

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) noexcept = delete;
    Renderer& operator=(Renderer&&) noexcept = delete;

    void attach(Window*);
    void start();

private:
    void render();

    void draw_ui();
    void draw_ui_bounds(const std::shared_ptr<UI::Object>&);
    void draw_fps(IntPosition, i32, Color);

    Window* m_window { nullptr };
    Camera m_camera {};
    u64 m_frame { 0 };
};

}

#pragma once

#include <graphics/window.h>
#include <memory>
#include <string>
#include <ui/structs.h>
#include <util/types.h>

struct Camera2D;

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
    void draw_ui_bounds(UI::Object*);
    void draw_fps(IntPosition, i32, Color);

    Window* m_window { nullptr };
    std::unique_ptr<Camera2D> m_camera { nullptr };
    u64 m_frame { 0 };
};

}

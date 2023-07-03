#pragma once

#include <graphics/camera.h>
#include <graphics/render_layer.h>
#include <graphics/texture.h>
#include <graphics/window.h>

namespace calamus {

namespace UI {
    class Object;
}

class Renderer final {
public:
    // TODO get rid of these and find a more appropriate way allowing render layers to access
    //      crucial functionality (camera manipulation, draw calls, etc) without exposing these
    //      to other, unrelated parts of the engine.
    friend class WorldTestLayer;

    Renderer();
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) noexcept = delete;
    Renderer& operator=(Renderer&&) noexcept = delete;

    void attach(Window*);
    void start();

    using render_callback = std::function<void(u64)>;
    void install_prerender_callback(const render_callback&);

    enum class LayerSpace {
        WorldSpace,
        ScreenSpace,
    };
    template <typename Layer>
        requires std::is_base_of_v<RenderLayer, Layer>
    void install_layer(LayerSpace);

    [[nodiscard]] auto frame_count() const noexcept { return m_frame_count; }

    [[nodiscard]] auto& camera() noexcept { return m_camera; }
    [[nodiscard]] const auto& camera() const noexcept { return m_camera; }

    [[nodiscard]] const auto& window() const {
        VERIFY_PTR(m_window);
        return *m_window;
    }

private:
    void prepare_render();
    void notify_prerender_callbacks();
    void render_world_space_layers();
    void render_screen_space_layers();
    void finalize();

    void draw_texture(const calamus::Texture&, IntPosition);

    Window* m_window { nullptr };
    Camera m_camera {};
    u64 m_frame_count { 0 };

    std::vector<render_callback> m_prerender_callbacks {};
    // layers rendered in world space (i.e. affected by camera)
    std::vector<std::unique_ptr<RenderLayer>> m_world_space_layers {};
    // layers rendered in screen space (i.e. unaffected by camera)
    std::vector<std::unique_ptr<RenderLayer>> m_screen_space_layers {};
};

}

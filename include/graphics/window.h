#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <ui/structs.h>
#include <util/formatter.h>
#include <util/types.h>
#include <vector>

namespace calamus {

struct WindowProperties {
    bool should_close; // WindowShouldClose()
    bool ready; // IsWindowReady()
    bool fullscreen; // IsWindowFullscreen(), ToggleFullscreen()
    bool hidden; // IsWindowHidden()
    bool maximized; // IsWindowMaximized(), MaximizeWindow()
    bool minimized; // IsWindowMinimized(), MinimizeWindow()
    bool focused; // IsWindowFocused()
    bool cursor_visible; // ShowCursor(), HideCursor()
    bool cursor_enabled; // EnableCursor(), DisableCursor()
    IntSize min_size; // SetWindowMinSize()
    IntSize size; // [GetScreenWidth(), GetScreenHeight()]
    IntPosition position; // SetWindowPosition(int, int)
    i32 monitor; // SetWindowMonitor(int)
};

class Window final {
public:
    Window() = default;
    ~Window() = default;

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) noexcept = delete;
    Window& operator=(Window&&) noexcept = delete;

    void init();
    void refresh();
    void close();

    void set_title(std::string_view);

    void set_fullscreen(bool);

    void maximize();
    void minimize();

    void set_cursor_visible(bool);
    void set_cursor_enabled(bool);

    void set_minimum_size(IntSize);
    void set_size(IntSize);
    void set_resizable(bool);

    void set_position(IntPosition);

    void set_monitor(i32);

    using resize_callback = std::function<void(IntSize)>;
    void install_resize_callback(const resize_callback&);
    using move_callback = std::function<void(IntPosition)>;
    void install_move_callback(const move_callback&);

    [[nodiscard]] auto should_close() const noexcept { return m_properties.should_close; }
    [[nodiscard]] auto is_ready() const noexcept { return m_properties.ready; }
    [[nodiscard]] auto is_fullscreen() const noexcept { return m_properties.fullscreen; }
    [[nodiscard]] auto is_hidden() const noexcept { return m_properties.hidden; }
    [[nodiscard]] auto is_maximized() const noexcept { return m_properties.maximized; }
    [[nodiscard]] auto is_minimized() const noexcept { return m_properties.minimized; }
    [[nodiscard]] auto is_focused() const noexcept { return m_properties.focused; }
    [[nodiscard]] auto is_cursor_visible() const noexcept { return m_properties.cursor_visible; }
    [[nodiscard]] auto is_cursor_enabled() const noexcept { return m_properties.cursor_enabled; }
    [[nodiscard]] auto minimum_size() const noexcept { return m_properties.min_size; }
    [[nodiscard]] auto size() const noexcept { return m_properties.size; }
    [[nodiscard]] auto position() const noexcept { return m_properties.position; }
    [[nodiscard]] auto monitor() const noexcept { return m_properties.monitor; }
    [[nodiscard]] const auto& properties() const noexcept { return m_properties; }

private:
    WindowProperties m_properties {};
    std::string_view m_title { "Window" };

    std::vector<resize_callback> m_resize_callbacks {};
    std::vector<move_callback> m_move_callbacks {};
};

}

// this sucks. this could surely be automated.
FORMATTER(calamus::WindowProperties, "WindowProperties(should_close={},\n\tready={},\n\tfullscreen={},\n\thidden={},\n\tmaximized={},\n\tminimized={},\n\tfocused={},\n\tcursor_visible={},\n\tcursor_enabled={},\n\tmin_size={},\n\tsize={},\n\tposition={},\n\tmonitor={})", value.should_close, value.ready, value.fullscreen, value.hidden, value.maximized, value.minimized, value.focused, value.cursor_visible, value.cursor_enabled, value.min_size, value.size, value.position, value.monitor)

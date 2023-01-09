#include <algorithm>
#include <graphics/window.h>
#include <raylib.h>
#include <resources/state.h>
#include <util/assert.h>
#include <util/logging.h>
#include <util/raylib/raylib_extensions.h>

namespace calamus {

void Window::init() {
    const auto* config = VERIFY_PTR(state.config);
    InitWindow(config->width, config->height, m_title.data());
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    const auto monitor = GetCurrentMonitor();
    if (config->vsync) {
        const auto refresh_rate = GetMonitorRefreshRate(monitor);
        SetTargetFPS(refresh_rate);
    } else if (config->limit_fps) {
        SetTargetFPS(config->target_fps);
    } else {
        SetTargetFPS(0);
    }

    m_properties.cursor_enabled = true;
    m_properties.min_size = IntSize { 0, 0 }; // I guess this is the default?
    m_properties.opacity = 1.0f;
    m_properties.monitor = monitor;
}

void Window::refresh() {
    m_properties.should_close = WindowShouldClose();
    m_properties.ready = IsWindowReady();
    m_properties.fullscreen = IsWindowFullscreen();
    m_properties.hidden = IsWindowHidden();
    m_properties.maximized = IsWindowMaximized();
    m_properties.minimized = IsWindowMinimized();
    m_properties.focused = IsWindowFocused();
    m_properties.cursor_visible = !IsCursorHidden();
    // cursor_enabled, opacity and min_size are not tracked by raylib

    const auto current_size = m_properties.size;
    const auto new_size = IntSize { GetScreenWidth(), GetScreenHeight() };
    if (new_size != current_size) {
        for (const auto& callback : m_resize_callbacks) {
            (*callback)(new_size);
        }
        m_properties.size = new_size;
    }

    const auto current_position = m_properties.position;
    const auto new_position = ca_pos_from(GetWindowPosition());
    if (new_position != current_position) {
        for (const auto& callback : m_move_callbacks) {
            (*callback)(new_position);
        }
        m_properties.position = new_position;
    }
}

void Window::close() { CloseWindow(); }

void Window::set_title(std::string_view title) {
    m_title = title;
    SetWindowTitle(m_title.data());
}

void Window::set_fullscreen(bool fullscreen) {
    if (fullscreen != m_properties.fullscreen)
        ToggleFullscreen();
    m_properties.fullscreen = fullscreen;
}

void Window::maximize() {
    m_properties.maximized = true;
    m_properties.minimized = false;
    MaximizeWindow();
}

void Window::minimize() {
    m_properties.minimized = true;
    m_properties.maximized = false;
    MinimizeWindow();
}

void Window::set_cursor_visible(bool visible) {
    m_properties.cursor_visible = visible;
    if (visible)
        ShowCursor();
    else
        HideCursor();
}

void Window::set_cursor_enabled(bool enabled) {
    m_properties.cursor_enabled = enabled;
    if (enabled)
        EnableCursor();
    else
        DisableCursor();
}

void Window::set_minimum_size(IntSize size) {
    m_properties.min_size = size;
    SetWindowMinSize(size.width, size.height);
}

void Window::set_size(IntSize size) {
    m_properties.size = size;
    SetWindowSize(size.width, size.height);
}

void Window::set_position(IntPosition position) {
    m_properties.position = position;
    SetWindowPosition(position.x, position.y);
}

void Window::set_monitor(i32 monitor_index) {
    m_properties.monitor = monitor_index;
    SetWindowMonitor(monitor_index);
}

void Window::set_opacity(f32 opacity) {
    m_properties.opacity = opacity;
    // SetWindowOpacity(opacity);
    // For some reason, even though this function is documented on the website,
    // it is not actually present. Weird. https://www.raylib.com/cheatsheet/cheatsheet.html
}

void Window::install_resize_callback(resize_callback callback) {
    m_resize_callbacks.emplace_back(callback);
}

void Window::install_move_callback(move_callback callback) {
    m_move_callbacks.emplace_back(callback);
}

}

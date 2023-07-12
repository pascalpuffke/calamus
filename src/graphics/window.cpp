#include <algorithm>
#include <graphics/window.h>
#include <resources/state.h>
#include <util/assert.h>
#include <util/raylib/raylib_wrapper.h>

namespace calamus {

static constexpr u32 FLAG_RESIZABLE = 0x04;

using namespace wrapper;

void Window::init() {
    const auto* config = VERIFY_PTR(state.config);
    rcore::init_window(IntSize { config->width, config->height }, m_title);
    rcore::set_window_state(FLAG_RESIZABLE);

    const auto monitor = rcore::get_current_monitor();
    const auto refresh = rcore::get_monitor_refresh_rate(monitor);
    LOG_INFO("Using monitor '{}', {} Hz, {}", rcore::get_monitor_name(monitor), refresh, rcore::get_monitor_size(monitor));

    if (config->vsync) {
        rcore::set_target_fps(refresh);
    } else if (config->limit_fps) {
        rcore::set_target_fps(config->target_fps);
    } else {
        rcore::set_target_fps(0);
    }

    m_properties.cursor_enabled = true;
    m_properties.min_size = IntSize { 0, 0 }; // I guess this is the default?
    m_properties.monitor = monitor;
}

void Window::refresh() {
    m_properties.should_close = rcore::window_should_close();
    m_properties.ready = rcore::is_window_ready();
    m_properties.fullscreen = rcore::is_window_fullscreen();
    m_properties.hidden = rcore::is_window_hidden();
    m_properties.maximized = rcore::is_window_maximized();
    m_properties.minimized = rcore::is_window_minimized();
    m_properties.focused = rcore::is_window_focused();
    m_properties.cursor_visible = !rcore::is_cursor_hidden();
    // cursor_enabled, opacity and min_size are not tracked by raylib

    notify_callbacks_if_needed(rcore::get_screen_size(), rcore::get_window_position());
}

void Window::close() { rcore::close_window(); }

void Window::hide_os_cursor() { rcore::hide_cursor(); }

void Window::set_title(std::string_view title) {
    m_title = title;
    rcore::set_window_title(title);
}

void Window::set_fullscreen(bool fullscreen) {
    if (fullscreen != m_properties.fullscreen)
        rcore::toggle_fullscreen();
    m_properties.fullscreen = fullscreen;
}

void Window::maximize() {
    m_properties.maximized = true;
    m_properties.minimized = false;
    rcore::maximize_window();
}

void Window::minimize() {
    m_properties.minimized = true;
    m_properties.maximized = false;
    rcore::minimize_window();
}

void Window::set_cursor_visible(bool visible) {
    m_properties.cursor_visible = visible;
    if (visible)
        rcore::show_cursor();
    else
        rcore::hide_cursor();
}

void Window::set_cursor_enabled(bool enabled) {
    m_properties.cursor_enabled = enabled;
    if (enabled)
        rcore::enable_cursor();
    else
        rcore::disable_cursor();
}

void Window::set_minimum_size(IntSize size) {
    m_properties.min_size = size;
    rcore::set_window_min_size(size);
}

void Window::set_size(IntSize size) {
    notify_callbacks_if_needed(size, position());
    m_properties.size = size;
    rcore::set_window_size(size);
}

void Window::set_resizable(bool resizable) {
    if (resizable) {
        rcore::set_window_state(~FLAG_RESIZABLE);
    } else {
        rcore::set_window_state(FLAG_RESIZABLE);
    }
}

void Window::set_position(IntPosition position) {
    notify_callbacks_if_needed(size(), position);
    m_properties.position = position;
    rcore::set_window_position(position);
}

void Window::set_monitor(i32 monitor_index) {
    m_properties.monitor = monitor_index;
    rcore::set_window_monitor(monitor_index);
}

void Window::install_resize_callback(const resize_callback& callback) {
    m_resize_callbacks.emplace_back(callback);
}

void Window::install_move_callback(const move_callback& callback) {
    m_move_callbacks.emplace_back(callback);
}

void Window::notify_callbacks_if_needed(IntSize new_size, IntPosition new_position) {
    const auto current_size = m_properties.size;
    if (new_size != current_size) {
        for (const auto& callback : m_resize_callbacks) {
            callback(new_size);
        }
        m_properties.size = new_size;
    }

    const auto current_position = m_properties.position;
    if (new_position != current_position) {
        for (const auto& callback : m_move_callbacks) {
            callback(new_position);
        }
        m_properties.position = new_position;
    }
}

}

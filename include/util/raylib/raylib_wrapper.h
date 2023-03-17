#pragma once

#include <filesystem>
#include <graphics/camera.h>
#include <graphics/texture.h>
#include <input/key.h>
#include <resources/fonts.h>
#include <span>
#include <ui/structs.h>

/*
 * To reduce confusion, naming issues and stupid conversions between our types and raylib's bare C structs,
 * we want to avoid including raylib.h as much as possible.
 * These wrapper functions expose *only* our own types.
 */

namespace calamus::wrapper {

void draw_rect_with_outline(const IntRect&, Color background, Color outline, f32 thickness = 1.0f);

namespace rcore {
    // Window
    void init_window(IntSize, std::string_view title);
    bool window_should_close();
    void close_window();
    bool is_window_ready();
    bool is_window_fullscreen();
    bool is_window_hidden();
    bool is_window_minimized();
    bool is_window_maximized();
    bool is_window_focused();
    bool is_window_resized();
    bool is_window_state(u32);
    void set_window_state(u32);
    void clear_window_state(u32);
    void toggle_fullscreen();
    void maximize_window();
    void minimize_window();
    void restore_window();
    void set_window_icon(const std::filesystem::path&);
    void set_window_title(std::string_view);
    void set_window_position(IntPosition);
    void set_window_monitor(i32);
    void set_window_min_size(IntSize);
    void set_window_size(IntSize);
    IntPosition get_window_position();
    Size<f32> get_window_scale_dpi();

    IntSize get_screen_size();
    IntSize get_render_size();

    // Monitor
    i32 get_monitor_count();
    i32 get_current_monitor();
    IntPosition get_monitor_position(i32);
    IntSize get_monitor_size(i32);
    IntSize get_monitor_physical_size(i32);
    i32 get_monitor_refresh_rate(i32);
    std::string_view get_monitor_name(i32);

    // Cursor
    void show_cursor();
    void hide_cursor();
    bool is_cursor_hidden();
    void enable_cursor();
    void disable_cursor();
    bool is_cursor_on_screen();

    // Basic drawing
    void clear_background(Color);
    void begin_drawing();
    void end_drawing();
    void begin_mode_2d(const Camera&);
    void end_mode_2d();

    // Input
    bool is_key_pressed(Key);
    bool is_key_down(Key);
    bool is_key_released(Key);
    bool is_key_up(Key);

    bool is_mouse_button_pressed(MouseButton);
    bool is_mouse_button_down(MouseButton);
    bool is_mouse_button_released(MouseButton);
    bool is_mouse_button_up(MouseButton);
    void set_mouse_position(IntPosition);
    void set_mouse_offset(IntPosition);
    void set_mouse_scale(Size<f32>);
    void set_mouse_cursor(MouseCursor);
    IntPosition get_mouse_position();
    IntPosition get_mouse_delta();
    f32 get_mouse_wheel_move();

    bool is_gamepad_available(i32);
    std::string_view get_gamepad_name(i32);
    bool is_gamepad_button_pressed(i32, i32 button);
    bool is_gamepad_button_released(i32, i32 button);
    bool is_gamepad_button_down(i32, i32 button);
    bool is_gamepad_button_up(i32, i32 button);
    i32 get_gamepad_last_button();
    i32 get_gamepad_axis_count(i32);
    f32 get_gamepad_axis_movement(i32, i32 axis);
    i32 set_gamepad_mappings(std::string_view);

    // Timing
    void set_target_fps(i32);
    i32 get_fps();
    f32 get_frame_time();
    f64 get_elapsed_time();

    // Misc. functions
    void take_screenshot(const std::filesystem::path&);
    void open_url(std::string_view);
}

namespace rshapes {
    void draw_pixel(IntPosition, Color);
    void draw_line(IntPosition start, IntPosition end, f32 thickness, Color);
    void draw_circle(IntPosition center, f32 radius, Color);
    void draw_circle_outline(IntPosition center, f32 radius, Color);
    void draw_rectangle(const IntRect&, Color);
    void draw_rectangle_outline(const IntRect&, f32 thickness, Color);
    void draw_triangle(IntPosition p1, IntPosition p2, IntPosition p3, Color);
    void draw_triangle_outline(IntPosition p1, IntPosition p2, IntPosition p3, Color);
}

namespace rtext {
    Font load_font(const std::filesystem::path&);
    void unload_font(Font);
    IntSize measure_text(std::string_view text, i32 size, i32 spacing, Resources::FontType font_type);
    void draw_text(std::string_view text, IntPosition position, i32 size, Color color, Resources::FontType font_type = Resources::FontType::Regular);
}

namespace rtextures {
    void unload_texture(const calamus::Texture&);
    void draw_texture(const calamus::Texture&, IntPosition, IntSize);
}

}

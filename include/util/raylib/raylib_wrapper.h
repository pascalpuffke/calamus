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

void draw_rect_with_outline(const IntRect& rect, Color background, Color outline, f32 thickness = 1.0f);

namespace rcore {
    void clear_background(Color);
    void begin_drawing();
    void end_drawing();
    void begin_mode_2d(const Camera&);
    void end_mode_2d();
    IntPosition get_window_position();
    bool is_key_pressed(Key);
    bool is_key_down(Key);
    bool is_key_released(Key);
    bool is_key_Up(Key);
    bool is_mouse_button_pressed(MouseButton);
    bool is_mouse_button_down(MouseButton);
    bool is_mouse_button_released(MouseButton);
    bool is_mouse_button_up(MouseButton);
    IntPosition get_mouse_position();
    IntPosition get_mouse_delta();
    void set_mouse_position(IntPosition);
    void set_mouse_offset(IntPosition);
    void set_mouse_scale(Size<f32>);
    f32 get_mouse_wheel_move();
    void set_mouse_cursor(MouseCursor);
    void set_target_fps(i32);
    i32 get_fps();
    f32 get_frame_time();
}

namespace rshapes {
    void draw_pixel(IntPosition position, Color color);
    void draw_line(IntPosition start, IntPosition end, f32 thickness, Color color);
    void draw_circle(IntPosition center, f32 radius, Color color);
    void draw_circle_outline(IntPosition center, f32 radius, Color color);
    void draw_rectangle(const IntRect& rectangle, Color color);
    void draw_rectangle_outline(const IntRect& rectangle, f32 thickness, Color color);
    void draw_triangle(IntPosition p1, IntPosition p2, IntPosition p3, Color color);
    void draw_triangle_outline(IntPosition p1, IntPosition p2, IntPosition p3, Color color);
}

namespace rtext {
    Font load_font(const std::filesystem::path&);
    void unload_font(Font);
    IntSize measure_text(std::string_view text, i32 size, i32 spacing, Resources::FontType font_type);
    void draw_text(std::string_view text, IntPosition position, i32 size, Color color, Resources::FontType font_type = Resources::FontType::Regular);
}

namespace rtextures {
    using texture_id = u32;

    texture_id upload_texture(std::span<u8> image_data, IntSize size, i32 mipmaps, i32 format);
    void unload_texture(const calamus::Texture& texture);
}

}

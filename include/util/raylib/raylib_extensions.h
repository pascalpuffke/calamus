#pragma once

#include <raylib.h>
#include <resources/fonts.h>
#include <ui/structs.h>

// This looks really goofy and unnecessary, because it is.
// Maybe this could later be expandable to other types by then 'if constexpr'-checking types,
// but right now this doesn't need to be templated
template <typename T>
    requires std::is_same_v<std::remove_cvref_t<T>, calamus::Rectangle<i32>>
constexpr inline Rectangle rl_rect_from(const T& t) {
    return Rectangle {
        static_cast<decltype(Rectangle::x)>(t.x),
        static_cast<decltype(Rectangle::y)>(t.y),
        static_cast<decltype(Rectangle::width)>(t.width),
        static_cast<decltype(Rectangle::height)>(t.height)
    };
}

template <typename T, typename U>
    requires std::is_same_v<std::remove_cvref_t<T>, calamus::Position<i32>> && std::is_same_v<std::remove_cvref_t<U>, calamus::Size<i32>>
constexpr inline Rectangle rl_rect_from(const T& t, const T& u) {
    return Rectangle {
        static_cast<decltype(Rectangle::x)>(t.x),
        static_cast<decltype(Rectangle::y)>(t.y),
        static_cast<decltype(Rectangle::width)>(u.width),
        static_cast<decltype(Rectangle::height)>(u.height)
    };
}

inline Vector2 rl_vec_from(const calamus::IntPosition& pos) {
    return Vector2 {
        static_cast<decltype(Vector2::x)>(pos.x),
        static_cast<decltype(Vector2::y)>(pos.y),
    };
}

inline calamus::IntPosition ca_pos_from(const Vector2& vec) {
    return calamus::IntPosition {
        static_cast<i32>(vec.x),
        static_cast<i32>(vec.y),
    };
}

inline calamus::IntSize ca_size_from(const Vector2& vec) {
    return calamus::IntSize {
        static_cast<i32>(vec.x),
        static_cast<i32>(vec.y),
    };
}

inline Color rl_color_from(const calamus::Color& color) {
    return Color {
        color.r,
        color.g,
        color.b,
        color.a,
    };
}

namespace wrapper {

namespace rcore {
    // void ClearBackground(Color color)
    inline void clear_background(const calamus::Color& color) {
        ClearBackground(rl_color_from(color));
    }

    // void BeginDrawing()
    inline void begin_drawing() {
        BeginDrawing();
    }

    // void EndDrawing()
    inline void end_drawing() {
        EndDrawing();
    }

    // void BeginMode2D(Camera2D)
    inline void begin_mode_2d(const Camera2D& camera) {
        BeginMode2D(camera);
    }

    // void EndMode2D()
    inline void end_mode_2d() {
        EndMode2D();
    }

    // bool IsKeyPressed(int)
    inline bool is_key_pressed(i32 key) { // TODO this shouldn't be an int
        return IsKeyPressed(key);
    }

    // bool IsKeyDown(int)
    inline bool is_key_down(i32 key) { // TODO this shouldn't be an int
        return IsKeyDown(key);
    }

    // bool IsKeyReleased(int)
    inline bool is_key_released(i32 key) { // TODO this shouldn't be an int
        return IsKeyReleased(key);
    }

    // bool IsKeyUp(int)
    inline bool is_key_Up(i32 key) { // TODO this shouldn't be an int
        return IsKeyUp(key);
    }

    // bool IsMouseButtonPressed(int)
    inline bool is_mouse_button_pressed(calamus::MouseButton button) {
        return IsMouseButtonPressed(static_cast<i32>(button));
    }

    // bool IsMouseButtonDown(int)
    inline bool is_mouse_button_down(calamus::MouseButton button) {
        return IsMouseButtonDown(static_cast<i32>(button));
    }

    // bool IsMouseButtonReleased(int)
    inline bool is_mouse_button_released(calamus::MouseButton button) {
        return IsMouseButtonReleased(static_cast<i32>(button));
    }

    // bool IsMouseButtonUp(int)
    inline bool is_mouse_button_up(calamus::MouseButton button) {
        return IsMouseButtonUp(static_cast<i32>(button));
    }

    // Vector2 GetMousePosition()
    inline calamus::IntPosition get_mouse_position() {
        return ca_pos_from(GetMousePosition());
    }

    // Vector2 GetMouseDelta()
    inline calamus::IntPosition get_mouse_delta() {
        return ca_pos_from(GetMouseDelta());
    }

    // void SetMousePosition(int, int)
    inline void set_mouse_position(calamus::IntPosition position) {
        SetMousePosition(position.x, position.y);
    }

    // void SetMouseOffset(int, int)
    inline void set_mouse_offset(calamus::IntPosition offset) {
        SetMouseOffset(offset.x, offset.y);
    }

    // void SetMouseScale(float, float)
    inline void set_mouse_scale(calamus::Size<f32> scale) {
        SetMouseScale(scale.width, scale.height);
    }

    // Vector2 GetMouseWheelMove()
    inline f32 get_mouse_wheel_move() {
        return GetMouseWheelMove();
    }

    // void SetMouseCursor(int)
    inline void set_mouse_cursor(i32 cursor) { // TODO enum MouseCursor
        SetMouseCursor(cursor);
    }

    // void SetTargetFPS(int)
    inline void set_target_fps(i32 fps) {
        SetTargetFPS(fps);
    }

    // int GetFPS()
    inline i32 get_fps() {
        return GetFPS();
    }

    // float GetFrameTime()
    inline f32 get_frame_time() {
        return GetFrameTime();
    }
}

namespace rshapes {
    // void DrawPixelV(Vector2, Color)
    inline void draw_pixel(const calamus::IntPosition& position, const calamus::Color& color) {
        DrawPixelV(rl_vec_from(position), rl_color_from(color));
    }

    // void DrawLineEx(Vector2, Vector2, float, Color)
    inline void draw_line(const calamus::IntPosition& start, const calamus::IntPosition& end, f32 thickness, const calamus::Color& color) {
        DrawLineEx(rl_vec_from(start), rl_vec_from(end), thickness, rl_color_from(color));
    }

    // void DrawCircleV(Vector2, float, Color)
    inline void draw_circle(const calamus::IntPosition& center, f32 radius, const calamus::Color& color) {
        DrawCircleV(rl_vec_from(center), radius, rl_color_from(color));
    }

    // void DrawCircleLines(int, int, float, Color)
    inline void draw_circle_outline(const calamus::IntPosition& center, f32 radius, const calamus::Color& color) {
        DrawCircleLines(static_cast<i32>(center.x), static_cast<i32>(center.y), radius, rl_color_from(color));
    }

    // void DrawRectangleRec(Rectangle, Color)
    inline void draw_rectangle(const calamus::IntRect& rectangle, const calamus::Color& color) {
        DrawRectangleRec(rl_rect_from(rectangle), rl_color_from(color));
    }

    // void DrawRectangleLinesEx(Rectangle, float, Color)
    inline void draw_rectangle_outline(const calamus::IntRect& rectangle, f32 thickness, const calamus::Color& color) {
        DrawRectangleLinesEx(rl_rect_from(rectangle), thickness, rl_color_from(color));
    }

    // void DrawTriangle(Vector2, Vector2, Vector2, Color)
    inline void draw_triangle(const calamus::IntPosition& p1, const calamus::IntPosition& p2, const calamus::IntPosition& p3, const calamus::Color& color) {
        DrawTriangle(rl_vec_from(p1), rl_vec_from(p2), rl_vec_from(p3), rl_color_from(color));
    }

    // void DrawTriangleLines(Vector2, Vector2, Vector2, Color)
    inline void draw_triangle_outline(const calamus::IntPosition& p1, const calamus::IntPosition& p2, const calamus::IntPosition& p3, const calamus::Color& color) {
        DrawTriangleLines(rl_vec_from(p1), rl_vec_from(p2), rl_vec_from(p3), rl_color_from(color));
    }
}

namespace rtext {
    using calamus::Resources::FontManagement;

    // Font LoadFont(const char*)
    inline Font load_font(std::string_view filename) {
        return LoadFont(filename.data());
    }

    // void UnloadFont(Font)
    inline void unload_font(Font font) {
        UnloadFont(font);
    }

    // Vector2 MeasureTextEx(Font, const char*, float, float)
    inline calamus::IntSize measure_text(std::string_view text, i32 size, i32 spacing, calamus::Resources::FontType font_type) {
        const auto& font = FontManagement::get().get_font(font_type);
        return ca_size_from(MeasureTextEx(font, text.data(), static_cast<f32>(size), static_cast<f32>(spacing)));
    }

    // void DrawText(const char*, int, int, int, Color)
    inline void draw_text(std::string_view text, const calamus::IntPosition& position, i32 size, const calamus::Color& color, calamus::Resources::FontType font_type = calamus::Resources::FontType::Regular) {
        const auto& font = FontManagement::get().get_font(font_type);
        DrawTextPro(font, text.data(), rl_vec_from(position), Vector2 { 0, 0 }, 0.0f, static_cast<f32>(size), 0, rl_color_from(color));
    }
}

namespace rtextures {
    // void DrawTextureEx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint)
    inline void draw_texture(Texture2D texture, const calamus::IntPosition& position, f32 rotation = 0.0f, f32 scale = 1.0f, const calamus::Color& tint = calamus::default_palette::white) {
        DrawTextureEx(texture, rl_vec_from(position), rotation, scale, rl_color_from(tint));
    }
}

}

inline void draw_rect_with_outline(
    const calamus::IntRect& rect,
    const calamus::Color& background,
    const calamus::Color& outline,
    f32 thickness = 1.0f) {
    wrapper::rshapes::draw_rectangle(rect, background);
    wrapper::rshapes::draw_rectangle_outline(rect, thickness, outline);
}

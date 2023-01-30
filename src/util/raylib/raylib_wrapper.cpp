#include <raylib.h>
#include <util/raylib/raylib_wrapper.h>
#include <utility>

Rectangle rl_rect_from(const calamus::IntRect& t) {
    return Rectangle {
        static_cast<decltype(Rectangle::x)>(t.x),
        static_cast<decltype(Rectangle::y)>(t.y),
        static_cast<decltype(Rectangle::width)>(t.width),
        static_cast<decltype(Rectangle::height)>(t.height)
    };
}

Rectangle rl_rect_from(calamus::IntPosition t, calamus::IntSize u) {
    return Rectangle {
        static_cast<decltype(Rectangle::x)>(t.x),
        static_cast<decltype(Rectangle::y)>(t.y),
        static_cast<decltype(Rectangle::width)>(u.width),
        static_cast<decltype(Rectangle::height)>(u.height)
    };
}

Vector2 rl_vec_from(calamus::IntPosition pos) {
    return Vector2 {
        static_cast<decltype(Vector2::x)>(pos.x),
        static_cast<decltype(Vector2::y)>(pos.y),
    };
}

calamus::IntPosition ca_pos_from(const Vector2& vec) {
    return calamus::IntPosition {
        static_cast<i32>(vec.x),
        static_cast<i32>(vec.y),
    };
}

calamus::IntSize ca_size_from(const Vector2& vec) {
    return calamus::IntSize {
        static_cast<i32>(vec.x),
        static_cast<i32>(vec.y),
    };
}

Color rl_color_from(calamus::Color color) {
    return Color {
        color.r,
        color.g,
        color.b,
        color.a,
    };
}

namespace calamus::wrapper {

void draw_rect_with_outline(const IntRect& rect, Color background, Color outline, f32 thickness) {
    rshapes::draw_rectangle(rect, background);
    rshapes::draw_rectangle_outline(rect, thickness, outline);
}

namespace rcore {
    void clear_background(Color color) {
        ClearBackground(rl_color_from(color));
    }

    void begin_drawing() {
        BeginDrawing();
    }

    void end_drawing() {
        EndDrawing();
    }

    void begin_mode_2d(const Camera& camera) {
        BeginMode2D(Camera2D {
            .offset = rl_vec_from(camera.offset()),
            .target = rl_vec_from(camera.target()),
            .rotation = camera.rotation(),
            .zoom = camera.zoom(),
        });
    }

    void end_mode_2d() {
        EndMode2D();
    }

    IntPosition get_window_position() {
        return ca_pos_from(GetWindowPosition());
    }

    bool is_key_pressed(Key key) {
        return IsKeyPressed(static_cast<i32>(key));
    }

    bool is_key_down(Key key) {
        return IsKeyDown(static_cast<i32>(key));
    }

    bool is_key_released(Key key) {
        return IsKeyReleased(static_cast<i32>(key));
    }

    bool is_key_up(Key key) {
        return IsKeyUp(static_cast<i32>(key));
    }

    bool is_mouse_button_pressed(MouseButton button) {
        return IsMouseButtonPressed(std::to_underlying(button));
    }

    bool is_mouse_button_down(MouseButton button) {
        return IsMouseButtonDown(std::to_underlying(button));
    }

    bool is_mouse_button_released(MouseButton button) {
        return IsMouseButtonReleased(std::to_underlying(button));
    }

    bool is_mouse_button_up(MouseButton button) {
        return IsMouseButtonUp(std::to_underlying(button));
    }

    IntPosition get_mouse_position() {
        return ca_pos_from(GetMousePosition());
    }

    IntPosition get_mouse_delta() {
        return ca_pos_from(GetMouseDelta());
    }

    void set_mouse_position(IntPosition position) {
        SetMousePosition(position.x, position.y);
    }

    void set_mouse_offset(IntPosition offset) {
        SetMouseOffset(offset.x, offset.y);
    }

    void set_mouse_scale(Size<f32> scale) {
        SetMouseScale(scale.width, scale.height);
    }

    f32 get_mouse_wheel_move() {
        return GetMouseWheelMove();
    }

    void set_mouse_cursor(MouseCursor cursor) {
        SetMouseCursor(std::to_underlying(cursor));
    }

    void set_target_fps(i32 fps) {
        SetTargetFPS(fps);
    }

    i32 get_fps() {
        return GetFPS();
    }

    f32 get_frame_time() {
        return GetFrameTime();
    }
}

namespace rshapes {
    void draw_pixel(IntPosition position, Color color) {
        DrawPixelV(rl_vec_from(position), rl_color_from(color));
    }

    void draw_line(IntPosition start, IntPosition end, f32 thickness, Color color) {
        DrawLineEx(rl_vec_from(start), rl_vec_from(end), thickness, rl_color_from(color));
    }

    void draw_circle(IntPosition center, f32 radius, Color color) {
        DrawCircleV(rl_vec_from(center), radius, rl_color_from(color));
    }

    void draw_circle_outline(IntPosition center, f32 radius, Color color) {
        DrawCircleLines(static_cast<i32>(center.x), static_cast<i32>(center.y), radius, rl_color_from(color));
    }

    void draw_rectangle(const IntRect& rectangle, Color color) {
        DrawRectangleRec(rl_rect_from(rectangle), rl_color_from(color));
    }

    void draw_rectangle_outline(const IntRect& rectangle, f32 thickness, Color color) {
        DrawRectangleLinesEx(rl_rect_from(rectangle), thickness, rl_color_from(color));
    }

    void draw_triangle(IntPosition p1, IntPosition p2, IntPosition p3, Color color) {
        DrawTriangle(rl_vec_from(p1), rl_vec_from(p2), rl_vec_from(p3), rl_color_from(color));
    }

    void draw_triangle_outline(IntPosition p1, IntPosition p2, IntPosition p3, Color color) {
        DrawTriangleLines(rl_vec_from(p1), rl_vec_from(p2), rl_vec_from(p3), rl_color_from(color));
    }
}

namespace rtext {
    Font load_font(const std::filesystem::path& path) {
        return LoadFont(path.c_str());
    }

    void unload_font(Font font) {
        UnloadFont(font);
    }

    IntSize measure_text(std::string_view text, i32 size, i32 spacing, Resources::FontType font_type) {
        const auto& font = VERIFY_PTR(state.font_manager)->get_font(font_type);
        return ca_size_from(MeasureTextEx(font, text.data(), static_cast<f32>(size), static_cast<f32>(spacing)));
    }

    void draw_text(std::string_view text, IntPosition position, i32 size, Color color, Resources::FontType font_type) {
        const auto& font = VERIFY_PTR(state.font_manager)->get_font(font_type);
        DrawTextPro(font, text.data(), rl_vec_from(position), Vector2 { 0, 0 }, 0.0f, static_cast<f32>(size), 0, rl_color_from(color));
    }
}

namespace rtextures {
    void unload_texture(const calamus::Texture& texture) {
        // UnloadTexture only needs the ID, so passing an otherwise empty struct works fine.
        UnloadTexture(Texture2D {
            .id = texture.id(),
        });
    }
}

}

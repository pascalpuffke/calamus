#include <raylib.h>
#include <resources/state.h>
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

namespace rcore {
    void init_window(IntSize size, std::string_view title) {
        InitWindow(size.width, size.height, title.data());
    }
    bool window_should_close() { return WindowShouldClose(); }
    void close_window() { CloseWindow(); }
    bool is_window_ready() { return IsWindowReady(); }
    bool is_window_fullscreen() { return IsWindowFullscreen(); }
    bool is_window_hidden() { return IsWindowHidden(); }
    bool is_window_minimized() { return IsWindowMinimized(); }
    bool is_window_maximized() { return IsWindowMaximized(); }
    bool is_window_focused() { return IsWindowFocused(); }
    bool is_window_resized() { return IsWindowResized(); }
    bool is_window_state(u32 flag) { return IsWindowState(flag); }
    void set_window_state(u32 flags) { return SetWindowState(flags); }
    void clear_window_state(u32 flags) { return ClearWindowState(flags); }
    void toggle_fullscreen() { ToggleFullscreen(); }
    void maximize_window() { MaximizeWindow(); }
    void minimize_window() { MinimizeWindow(); }
    void restore_window() { RestoreWindow(); }
    void set_window_icon(const std::filesystem::path& icon_path) {
        VERIFY(exists(icon_path));
        VERIFY(is_regular_file(icon_path));

        auto image = LoadImage(icon_path.c_str());
        VERIFY(image.width > 0);
        SetWindowIcon(image);
        UnloadImage(image);
    }
    void set_window_title(std::string_view title) { SetWindowTitle(title.data()); }
    void set_window_position(IntPosition position) { SetWindowPosition(position.x, position.y); }
    void set_window_monitor(i32 monitor) { SetWindowMonitor(monitor); }
    void set_window_min_size(IntSize min_size) { SetWindowMinSize(min_size.width, min_size.height); }
    void set_window_size(IntSize size) { SetWindowSize(size.width, size.height); }
    IntPosition get_window_position() { return ca_pos_from(GetWindowPosition()); }
    Size<f32> get_window_scale_dpi() {
        const auto scale = GetWindowScaleDPI();
        return Size<f32> { scale.x, scale.y };
    }

    IntSize get_screen_size() { return IntSize { GetScreenWidth(), GetScreenHeight() }; }
    IntSize get_render_size() { return IntSize { GetRenderWidth(), GetRenderHeight() }; }

    i32 get_monitor_count() { return GetMonitorCount(); }
    i32 get_current_monitor() { return GetCurrentMonitor(); }
    IntPosition get_monitor_position(i32 monitor) { return ca_pos_from(GetMonitorPosition(monitor)); }
    IntSize get_monitor_size(i32 monitor) {
        return IntSize { GetMonitorWidth(monitor), GetMonitorHeight(monitor) };
    }
    IntSize get_monitor_physical_size(i32 monitor) {
        return IntSize { GetMonitorPhysicalWidth(monitor), GetMonitorPhysicalHeight(monitor) };
    }
    i32 get_monitor_refresh_rate(i32 monitor) { return GetMonitorRefreshRate(monitor); }
    std::string_view get_monitor_name(i32 monitor) {
        return std::string_view { GetMonitorName(monitor) };
    }

    void show_cursor() { ShowCursor(); }
    void hide_cursor() { HideCursor(); }
    bool is_cursor_hidden() { return IsCursorHidden(); }
    void enable_cursor() { EnableCursor(); }
    void disable_cursor() { DisableCursor(); }
    bool is_cursor_on_screen() { return IsCursorOnScreen(); }

    void clear_background(Color color) {
        ClearBackground(rl_color_from(color));
    }
    void begin_drawing() { BeginDrawing(); }
    void end_drawing() { EndDrawing(); }
    void begin_mode_2d(const Camera& camera) {
        BeginMode2D(Camera2D {
            .offset = rl_vec_from(camera.offset()),
            .target = rl_vec_from(camera.target()),
            .rotation = camera.rotation(),
            .zoom = camera.zoom(),
        });
    }
    void end_mode_2d() { EndMode2D(); }

    bool is_key_pressed(Key key) { return IsKeyPressed(std::to_underlying(key)); }
    bool is_key_down(Key key) { return IsKeyDown(std::to_underlying(key)); }
    bool is_key_released(Key key) { return IsKeyReleased(std::to_underlying(key)); }
    bool is_key_up(Key key) { return IsKeyUp(std::to_underlying(key)); }

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
    void set_mouse_position(IntPosition position) { SetMousePosition(position.x, position.y); }
    void set_mouse_offset(IntPosition offset) { SetMouseOffset(offset.x, offset.y); }
    void set_mouse_scale(Size<f32> scale) { SetMouseScale(scale.width, scale.height); }
    void set_mouse_cursor(MouseCursor cursor) { SetMouseCursor(std::to_underlying(cursor)); }
    IntPosition get_mouse_position() { return ca_pos_from(GetMousePosition()); }
    IntPosition get_mouse_delta() { return ca_pos_from(GetMouseDelta()); }
    f32 get_mouse_wheel_move() { return GetMouseWheelMove(); }

    bool is_gamepad_available(i32 gamepad) { return IsGamepadAvailable(gamepad); }
    std::string_view get_gamepad_name(i32 gamepad) { return GetGamepadName(gamepad); }
    bool is_gamepad_button_pressed(i32 gamepad, i32 button) { return IsGamepadButtonPressed(gamepad, button); }
    bool is_gamepad_button_released(i32 gamepad, i32 button) { return IsGamepadButtonReleased(gamepad, button); }
    bool is_gamepad_button_down(i32 gamepad, i32 button) { return IsGamepadButtonDown(gamepad, button); }
    bool is_gamepad_button_up(i32 gamepad, i32 button) { return IsGamepadButtonUp(gamepad, button); }
    i32 get_gamepad_last_button() { return GetGamepadButtonPressed(); }
    i32 get_gamepad_axis_count(i32 gamepad) { return GetGamepadAxisCount(gamepad); }
    f32 get_gamepad_axis_movement(i32 gamepad, i32 axis) { return GetGamepadAxisMovement(gamepad, axis); }
    i32 set_gamepad_mappings(std::string_view mappings) { return SetGamepadMappings(mappings.data()); }

    void set_target_fps(i32 fps) { SetTargetFPS(fps); }
    i32 get_fps() { return GetFPS(); }
    f32 get_frame_time() { return GetFrameTime(); }
    f64 get_elapsed_time() { return GetTime(); }

    void take_screenshot(const std::filesystem::path& screenshot_path) {
        TakeScreenshot(screenshot_path.c_str());
    }
    void open_url(std::string_view url) { OpenURL(url.data()); }
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
    Font load_font(const std::filesystem::path& path) { return LoadFont(path.c_str()); }
    void unload_font(Font font) { UnloadFont(font); }

    IntSize measure_text(std::string_view text, i32 size, i32 spacing, Resources::FontType font_type) {
        const auto& font = VERIFY(state.font_manager)->get_font(font_type);
        return ca_size_from(MeasureTextEx(font, text.data(), static_cast<f32>(size), static_cast<f32>(spacing)));
    }

    void draw_text(std::string_view text, IntPosition position, i32 size, Color color, Resources::FontType font_type) {
        const auto& font = VERIFY(state.font_manager)->get_font(font_type);
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

    void draw_texture(const calamus::Texture& texture, IntPosition position, IntSize size) {
        const auto original_width = texture.parent_width() == 0 ? texture.width() : texture.parent_width();
        const auto original_height = texture.parent_height() == 0 ? texture.height() : texture.parent_height();

        const auto rl_texture = Texture2D {
            .id = texture.id(),
            .width = original_width,
            .height = original_height,
            // These two don't matter
            .mipmaps = 0,
            .format = 0,
        };
        const auto source = IntRect { texture.offset().x, texture.offset().y, texture.width(), texture.height() };
        const auto dest = IntRect { position.x, position.y, size.width, size.height };
        const auto origin = IntPosition { 0 };
        const auto rotation = 0.0f;
        const auto tint = Color::from_hex(0xFFFFFF);

        DrawTexturePro(rl_texture, rl_rect_from(source), rl_rect_from(dest), rl_vec_from(origin), rotation, rl_color_from(tint));
    }
}

}

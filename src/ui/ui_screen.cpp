#include <graphics/renderer.h>
#include <resources/state.h>
#include <ui/ui_screen.h>
#include <util/raylib/raylib_wrapper.h>

namespace calamus::UI {

using namespace wrapper;

template <Arithmetic T = i32>
[[nodiscard]] static constexpr bool in_bounds(Position<T> position, Rectangle<T> rect) noexcept {
    if (position.x < rect.x)
        return false;
    if (position.x > rect.x + rect.width)
        return false;
    if (position.y < rect.y)
        return false;
    return position.y <= rect.y + rect.height;
}

void ScreenManager::check_hover(IntPosition position) {
    // TODO: Do this in a proper front to back order (by highest z-index?)
    for (const auto& object : m_layouts[state.current_screen].children()) {
        if (!object->is_hoverable())
            continue;

        if (in_bounds(position, object->rect())) {
            object->on_hover_begin(position);
            m_last_hovered_object = object;

            // Only one object can be in a hovered state at a time
            return;
        }

        if (object == m_last_hovered_object) {
            object->on_hover_end();
            m_last_hovered_object = nullptr;
        }
    }
}

void ScreenManager::check_click(MouseButton mouse_button, IntPosition position) {
    for (const auto& object : m_layouts[state.current_screen].children()) {
        if (!object->is_clickable())
            continue;

        if (in_bounds(position, object->rect())) {
            object->on_click(mouse_button, position);
            return;
        }
    }
}

void ScreenManager::register_screen(Screen screen, ScreenLayout&& layout) {
    m_layouts[screen] = std::move(layout);
}

ScreenLayout& ScreenManager::layout(Screen screen) {
    auto result = m_layouts.find(screen);
    ASSERT_MSG(result != m_layouts.end(), "No layout registered for requested screen");

    // Is this dumb?
    return result->second;
}

ScreenManager::ScreenManager() {
    VERIFY_PTR(state.renderer)->install_prerender_callback([this](auto) {
        const auto mouse_position = rcore::get_mouse_position();
        check_hover(mouse_position);

        for (auto button_index = 0; button_index < std::to_underlying(MouseButton::__Count); button_index++) {
            const auto button = static_cast<MouseButton>(button_index);
            if (rcore::is_mouse_button_pressed(button))
                check_click(button, mouse_position);
        }
    });
}

void ScreenLayout::rebuild_layout(IntSize new_size) {
    m_layout->set_parent_rect(IntRect { IntPosition { 0, 0 }, new_size });
    m_layout->apply(m_children);

    for (auto& object : m_children) {
        // workaround for buttons using wrong colors before being hovered the first time
        object->on_hover_end();
    }
}

}

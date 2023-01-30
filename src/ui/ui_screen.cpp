#include <resources/state.h>
#include <ui/ui_screen.h>

namespace calamus::UI {

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
    for (const auto& object : m_layouts[state.current_screen].children()) {
        if (!object->is_hoverable())
            continue;

        if (in_bounds(position, object->rect())) {
            object->on_hover_begin(position);
            // Only one object can be in a hovered state at a time
            // TODO: Do this in a proper front to back order (by highest z-index?)
            return;
        }
        // TODO: Call only once, and only if the object was previously hovered
        //       Right now this spams on_hover_end() to *all* hoverable objects on each frame
        object->on_hover_end();
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

const ScreenLayout& ScreenManager::layout(Screen screen) const {
    auto result = m_layouts.find(screen);
    ASSERT_MSG(result != m_layouts.end(), "No layout registered for requested screen");

    // Is this dumb?
    return result->second;
}

}

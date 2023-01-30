#pragma once

#include <functional>
#include <graphics/window.h>
#include <resources/state.h>
#include <ui/layout/layout.h>
#include <ui/ui_object.h>
#include <unordered_map>
#include <utility>
#include <vector>

namespace calamus::UI {

struct ScreenLayout {
    using object_ptr = std::shared_ptr<Object>;

    template <typename LayoutClass>
        requires std::is_base_of_v<Layout, LayoutClass>
    [[nodiscard]] static ScreenLayout create(std::vector<object_ptr>&& children) {
        const auto layout = std::make_unique<LayoutClass>();
        layout->set_parent_rect(IntRect { IntPosition { 0, 0 }, state.window->size() });
        layout->apply(children);

        for (auto& object : children)
            object->set_rect(layout->get(object));

        return ScreenLayout { std::move(children) };
    }

    [[nodiscard]] static ScreenLayout create(Layout& layout, std::vector<object_ptr>&& children) {
        layout.set_parent_rect(IntRect { IntPosition { 0, 0 }, state.window->size() });
        layout.apply(children);

        for (auto& object : children)
            object->set_rect(layout.get(object));

        return ScreenLayout { std::move(children) };
    }

    explicit ScreenLayout(std::vector<object_ptr> children)
        : m_children(std::move(children)) { }
    explicit ScreenLayout() = default;

    [[nodiscard]] const auto& children() const noexcept { return m_children; }

private:
    std::vector<object_ptr> m_children {};
};

class ScreenManager final {
public:
    ScreenManager() = default;
    ~ScreenManager() = default;

    ScreenManager(const ScreenManager&) = delete;
    ScreenManager& operator=(const ScreenManager&) = delete;
    ScreenManager(ScreenManager&&) noexcept = delete;
    ScreenManager& operator=(ScreenManager&&) noexcept = delete;

    void check_hover(IntPosition);
    void check_click(MouseButton, IntPosition);
    void register_screen(Screen, ScreenLayout&&);
    [[nodiscard]] const ScreenLayout& layout(Screen) const;

private:
    std::unordered_map<Screen, ScreenLayout> m_layouts;
};

}

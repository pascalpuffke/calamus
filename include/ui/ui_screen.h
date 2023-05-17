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

    ScreenLayout(std::unique_ptr<Layout> layout, std::vector<object_ptr> children)
        : m_layout(std::move(layout))
        , m_children(std::move(children)) { }
    explicit ScreenLayout() = default;

    template <typename LayoutClass>
        requires std::is_base_of_v<Layout, LayoutClass>
    [[nodiscard]] static ScreenLayout create(std::vector<object_ptr>&& children) {
        auto layout = std::make_unique<LayoutClass>();
        auto screen = ScreenLayout { std::move(layout), std::move(children) };

        screen.rebuild_layout();

        return screen;
    }

    [[nodiscard]] static ScreenLayout create(std::unique_ptr<Layout> layout, std::vector<object_ptr>&& children) {
        auto screen = ScreenLayout { std::move(layout), std::move(children) };

        screen.rebuild_layout();

        return screen;
    }

    void rebuild_layout(IntSize new_size = state.window->size()) {
        m_layout->set_parent_rect(IntRect { IntPosition { 0, 0 }, new_size });
        m_layout->apply(m_children);

        for (auto& object : m_children)
            object->set_rect(m_layout->get(object));
    }

    [[nodiscard]] const auto& children() const noexcept { return m_children; }

private:
    std::unique_ptr<Layout> m_layout { nullptr };
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
    [[nodiscard]] ScreenLayout& layout(Screen);

private:
    ScreenLayout::object_ptr m_last_hovered_object { nullptr };
    std::unordered_map<Screen, ScreenLayout> m_layouts;
};

}

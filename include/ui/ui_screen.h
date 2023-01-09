#pragma once

#include <functional>
#include <resources/state.h>
#include <ui/ui_object.h>
#include <unordered_map>
#include <utility>
#include <vector>

namespace calamus::UI {

struct ScreenLayout {
    using object_ptr = std::shared_ptr<Object>;

    [[nodiscard]] static ScreenLayout create(std::initializer_list<object_ptr> objects) {
        auto vector = std::vector<object_ptr> {};
        std::for_each(objects.begin(), objects.end(), [&](auto&& object) {
            vector.emplace_back(object);
        });
        return ScreenLayout { std::move(vector) };
    }

    std::vector<object_ptr> objects;

    explicit ScreenLayout() { }

private:
    explicit ScreenLayout(std::vector<object_ptr> new_objects)
        : objects(std::move(new_objects)) {
    }
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

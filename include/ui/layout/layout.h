#pragma once

#include <ui/structs.h>
#include <ui/ui_object.h>
#include <unordered_map>
#include <vector>

namespace calamus::UI {

class Layout {
public:
    using object_ptr = std::shared_ptr<Object>;

    explicit Layout(const IntRect& parent_rect)
        : m_parent_rect(parent_rect) { }
    Layout() = default;

    virtual ~Layout() = default;

    virtual void apply(const std::vector<object_ptr>&) = 0;

    virtual void set_parent_rect(const IntRect& parent_rect) {
        m_parent_rect = parent_rect;
    }

protected:
    std::unordered_map<object_ptr, IntRect> m_layout {};
    IntRect m_parent_rect {};
};

}
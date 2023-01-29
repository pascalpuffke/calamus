#include <ui/layout/fixed_layout.h>

namespace calamus::UI {

void FixedLayout::apply(const std::vector<object_ptr>& objects) {
    for (auto& object : objects) {
        m_layout[object] = object->rect();
    }
}

const IntRect& FixedLayout::get(object_ptr object) {
    const auto rect = m_layout.find(object);
    ASSERT(rect != m_layout.end());
    return rect->second;
}

}
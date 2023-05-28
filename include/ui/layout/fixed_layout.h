#pragma once

#include <ui/layout/layout.h>

namespace calamus::UI {

class FixedLayout final : public Layout {
public:
    // no-op
    void apply(const std::vector<object_ptr>&) override {}
};

}
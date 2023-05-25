#pragma once

#include <ui/layout/layout.h>

namespace calamus::UI {

class FixedLayout final : public Layout {
public:
    void apply(const std::vector<object_ptr>& objects) override;
};

}
#pragma once

#include <ui/structs.h>

namespace calamus::UI {

enum class Alignment {
    Center,
    CenterLeft,
    CenterRight,
    TopCenter,
    TopLeft,
    TopRight,
    BottomCenter,
    BottomLeft,
    BottomRight,
};

constexpr IntPosition calculate_offset_for_alignment(Alignment alignment, IntSize outer, IntSize inner) {
    auto offset = IntPosition { 0, 0 };
    switch (alignment) {
    case Alignment::Center: {
        offset.x = (outer.width - inner.width) / 2;
        offset.y = (outer.height - inner.height) / 2;
    } break;
    case Alignment::CenterLeft: {
        offset.y = (outer.height - inner.height) / 2;
    } break;
    case Alignment::CenterRight: {
        offset.x = outer.width - inner.width;
        offset.y = (outer.height - inner.height) / 2;
    } break;
    case Alignment::TopCenter: {
        offset.x = (outer.width - inner.width) / 2;
    } break;
    case Alignment::TopLeft: {
    } break;
    case Alignment::TopRight: {
        offset.x = outer.width - inner.width;
    } break;
    case Alignment::BottomCenter: {
        offset.x = (outer.width - inner.width) / 2;
        offset.y = outer.height - inner.height;
    } break;
    case Alignment::BottomLeft: {
        offset.y = outer.height - inner.height;
    } break;
    case Alignment::BottomRight: {
        offset.x = outer.width - inner.width;
        offset.y = outer.height - inner.height;
    } break;
    }
    return offset;
}

}
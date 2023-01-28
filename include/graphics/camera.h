#pragma once

#include <ui/structs.h>

namespace calamus {

class Camera final {
public:
    constexpr Camera() = default;
    constexpr Camera(IntPosition offset, IntPosition target, f32 rotation, f32 zoom)
        : m_offset(offset)
        , m_target(target)
        , m_rotation(rotation)
        , m_zoom(zoom) { }

    [[nodiscard]] constexpr auto offset() const noexcept { return m_offset; }
    [[nodiscard]] constexpr auto target() const noexcept { return m_target; }
    [[nodiscard]] constexpr auto rotation() const noexcept { return m_rotation; }
    [[nodiscard]] constexpr auto zoom() const noexcept { return m_zoom; }

    constexpr void set_offset(IntPosition offset) { m_offset = offset; }
    constexpr void set_target(IntPosition target) { m_target = target; }
    constexpr void set_rotation(f32 rotation) { m_rotation = rotation; }
    constexpr void set_zoom(f32 zoom) { m_zoom = zoom; }

private:
    IntPosition m_offset { 0, 0 };
    IntPosition m_target { 0, 0 };
    f32 m_rotation { 0.0f };
    f32 m_zoom { 1.0f };
};

}
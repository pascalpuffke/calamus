#pragma once

#include <memory>
#include <ui/structs.h>
#include <util/types.h>
#include <utility>

// do we really need these
#define UI_MAKE_HOVERABLE() \
public:                     \
    [[nodiscard]] virtual bool is_hoverable() const override { return true; }
#define UI_MAKE_CLICKABLE() \
public:                     \
    [[nodiscard]] virtual bool is_clickable() const override { return true; }

namespace calamus::UI {

enum class ObjectType {
    Label,
    Button,
    Unknown,
};

class Object {
public:
    Object() = default;
    virtual ~Object() = default;

    [[nodiscard]] virtual IntPosition position() { return m_position; }
    virtual void set_position(IntPosition position) { m_position = position; }

    [[nodiscard]] virtual IntSize size() { return m_size; }
    virtual void set_size(IntSize size) { m_size = size; }

    [[nodiscard]] virtual IntRect rect() { return IntRect { position(), size() }; }
    virtual void set_rect(const IntRect& rect) {
        set_size(rect.to_size());
        set_position(rect.to_position());
    }

    [[nodiscard]] virtual bool is_hoverable() const { return false; }
    virtual void on_hover_begin(IntPosition) {};
    // NOTE: This method is called on *every* frame on *all* objects that are not currently hovered.
    // You'd probably hope this is only called *once* as soon as the cursor stops hovering over
    // the last object, but alas that's not how it is implemented.
    virtual void on_hover_end() {};
    [[nodiscard]] virtual bool is_clickable() const { return false; }
    virtual void on_click(MouseButton, IntPosition) {};

    // Every child needs to override these.
    [[nodiscard]] virtual ObjectType type() const = 0;
    virtual void draw() = 0;

protected:
    IntPosition m_position {};
    IntSize m_size {};
};

}

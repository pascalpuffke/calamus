#pragma once

#include <assert.hpp>
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

class Button;
class Label;

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
    virtual void on_hover_end() {};
    [[nodiscard]] virtual bool is_clickable() const { return false; }
    virtual void on_click(MouseButton, IntPosition) {};

    // Every child needs to override these.
    [[nodiscard]] virtual ObjectType type() const = 0;
    virtual void draw() = 0;

    // A glorified/safer 'dynamic_cast'.
    template <typename T>
        requires std::is_base_of_v<Object, T>
    T* as_ptr() {
        // static assertions won't work due to Object not being constexpr, so we can't check the type in 'if constexpr'
        // It would be totally possible to convert a lot of the UI stuff to be constexpr, but I don't care enough.
        switch (type()) {
        case ObjectType::Button: {
            VERIFY((std::is_same_v<T, UI::Button>), "Cannot cast to object of different type");
        } break;
        case ObjectType::Label: {
            VERIFY((std::is_same_v<T, UI::Label>), "Cannot cast to object of different type");
        } break;
        default: {
            VERIFY(false, "Unknown object type");
        }
        }

        return static_cast<T*>(this);
    }

    template <typename T>
        requires std::is_base_of_v<Object, T>
    T& as_ref() {
        return *as_ptr<T>();
    }

protected:
    IntPosition m_position {};
    IntSize m_size {};
};

}

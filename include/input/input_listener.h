#pragma once

#include <ui/structs.h>

namespace calamus {

struct KeyEvent {
    i32 code;
};

struct MouseEvent {
    i32 button;
    IntPosition position;
};

class InputListener {
public:
    using key_callback = void (*)(KeyEvent);
    virtual void on_key_pressed(KeyEvent) = 0;
    virtual void on_key_down(KeyEvent) = 0;
    virtual void on_key_up(KeyEvent) = 0;
    virtual void on_key_released(KeyEvent) = 0;

    using mouse_callback = void (*)(MouseEvent);
    virtual void on_mouse_pressed(MouseEvent) = 0;
    virtual void on_mouse_down(MouseEvent) = 0;
    virtual void on_mouse_up(MouseEvent) = 0;
    virtual void on_mouse_released(MouseEvent) = 0;
};

}

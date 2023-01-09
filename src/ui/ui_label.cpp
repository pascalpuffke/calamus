#include <ui/ui_label.h>
#include <util/raylib/raylib_extensions.h>

namespace calamus::UI {

Label::Label(const char* text, i32 size, Color color, IntPosition position)
    : m_color(color)
    , m_text(text)
    , m_font_size(size) {
    m_position = position;
}

IntSize Label::size() {
    if (m_size.width == 0)
        m_size = wrapper::rtext::measure_text(text(), font_size(), spacing(), font_type());
    return m_size;
}

void Label::draw() {
    wrapper::rtext::draw_text(text(), position(), font_size(), color());
}

}
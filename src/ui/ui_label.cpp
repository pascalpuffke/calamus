#include <ui/ui_label.h>
#include <util/raylib/raylib_wrapper.h>

namespace calamus::UI {

void Label::remeasure_text() {
    m_size = wrapper::rtext::measure_text(text(), font_size(), spacing(), font_type());
}

Label::Label(std::string text, i32 size, Color color, IntPosition position)
    : m_color(color)
    , m_text(std::move(text))
    , m_font_size(size) {
    m_position = position;
}

IntSize Label::size() {
    if (m_size.width == 0)
        remeasure_text();
    return m_size;
}

ObjectType Label::type() const { return ObjectType::Label; }

void Label::set_text(const std::string& text) { m_text = text; }

std::string_view Label::text() const noexcept { return m_text; }

void Label::set_font_size(i32 font_size) {
    m_font_size = font_size;
    remeasure_text();
}

i32 Label::font_size() const noexcept { return m_font_size; }

void Label::set_font_type(Resources::FontType type) { m_font_type = type; }

Resources::FontType Label::font_type() const noexcept { return m_font_type; }

void Label::set_color(Color color) { m_color = color; }

Color Label::color() const noexcept { return m_color; }

void Label::draw() {
    wrapper::rtext::draw_text(text(), position(), font_size(), color());
}

}
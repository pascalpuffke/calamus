#include <ui/ui_button.h>
#include <util/raylib/raylib_wrapper.h>

namespace calamus::UI {

Button::Button(std::shared_ptr<Label>& label, IntPosition position, IntSize size, std::function<void(IntPosition)> on_click, Alignment label_alignment)
    : m_on_click(std::move(on_click))
    , m_label(label)
    , m_label_alignment(label_alignment) {
    m_position = position;
    m_size = size;

    const auto label_position = compute_label_position();
    m_label->set_position(label_position + position);
}

IntPosition Button::compute_label_position() {
    if (!m_label)
        return {};
    const auto margins = Size { 0, 0 };
    const auto usable_size = m_size - (margins * 2);
    const auto label_size = m_label->size();
    auto label_offset = IntPosition { 0, 0 };
    label_offset += calculate_offset_for_alignment(m_label_alignment, usable_size, label_size);
    label_offset += margins.to_position();

    return label_offset;
}

void Button::draw() {
    if (m_has_frame)
        wrapper::draw_rect_with_outline(rect(), m_hovered ? m_hover_color : m_background_color, m_outline_color);
    m_label->draw();
}

void Button::on_click(MouseButton mouse_button, IntPosition position) {
    if (mouse_button == MouseButton::Left)
        m_on_click(position);
}

void Button::set_position(IntPosition position) {
    Object::set_position(position);

    const auto label_position = compute_label_position();
    m_label->set_position(label_position + m_position);
}

void Button::set_size(IntSize size) {
    Object::set_size(size);

    const auto label_position = compute_label_position();
    m_label->set_position(label_position + m_position);
}

}

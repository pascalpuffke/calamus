#include <ui/ui_button.h>
#include <util/raylib/raylib_wrapper.h>

namespace calamus::UI {

IntPosition Button::compute_label_position() {
    if (!m_label)
        return {};
    const auto margins = Size { 0, 0 };
    const auto usable_size = m_size - (margins * 2);
    const auto label_size = m_label->size();
    auto label_offset = calculate_offset_for_alignment(m_label_alignment, usable_size, label_size);
    label_offset += margins.to_position();

    return label_offset;
}

void Button::reset_label() {
    const auto relative_label_position = compute_label_position();
    m_label->set_position(relative_label_position + position());
}

Button::Button(std::shared_ptr<Label>& label, IntPosition position, IntSize size, std::function<void(IntPosition)> on_click, Alignment label_alignment)
    : m_on_click(std::move(on_click))
    , m_label(label)
    , m_label_alignment(label_alignment) {
    m_position = position;
    m_size = size;
    reset_label();
}

ObjectType Button::type() const { return ObjectType::Button; }

void Button::set_position(IntPosition position) {
    Object::set_position(position);
    reset_label();
}

void Button::set_size(IntSize size) {
    Object::set_size(size);
    reset_label();
}

void Button::on_hover_begin(IntPosition) {
    m_hovered = true;
    if (m_label_hover_color.has_value())
        m_label->set_color(m_label_hover_color.value());
}

void Button::on_hover_end() {
    m_hovered = false;
    set_label_color(m_label_color);
}

void Button::on_click(MouseButton mouse_button, IntPosition position) {
    if (mouse_button == MouseButton::Left)
        m_on_click(position);
}

void Button::draw() {
    if (m_has_frame)
        wrapper::draw_rect_with_outline(rect(), m_hovered ? hover_color() : background_color(), outline_color());
    m_label->draw();
}

const std::shared_ptr<Label>& Button::label() const { return m_label; }

Color Button::label_color() const noexcept { return m_label_color; }

void Button::set_label_color(Color color) {
    m_label_color = color;
    m_label->set_color(color);
}

std::optional<Color> Button::label_hover_color() const noexcept { return m_label_hover_color; }

void Button::set_label_hover_color(Color color) { m_label_hover_color = color; }

Color Button::outline_color() const noexcept { return m_outline_color; }

void Button::set_outline_color(Color color) { m_outline_color = color; }

Color Button::background_color() const noexcept { return m_background_color; }

void Button::set_background_color(Color color) { m_background_color = color; }

Color Button::hover_color() const noexcept { return m_hover_color; }

void Button::set_hover_color(Color color) { m_hover_color = color; }
}

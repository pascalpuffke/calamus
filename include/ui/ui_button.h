#pragma once

#include <functional>
#include <ui/alignment.h>
#include <ui/ui_label.h>

namespace calamus::UI {

class Button final : public Object {
    UI_MAKE_HOVERABLE()
    UI_MAKE_CLICKABLE()
public:
    Button(std::shared_ptr<Label>& label, IntPosition position, IntSize size, std::function<void(IntPosition)> on_click, Alignment label_alignment = Alignment::Center);
    ~Button() override = default;

    static std::shared_ptr<Button> create(std::shared_ptr<Label>&& label, IntPosition position, IntSize size, std::function<void(IntPosition)>&& on_click, Alignment label_alignment = Alignment::Center) {
        return std::make_shared<Button>(label, position, size, on_click, label_alignment);
    }

    [[nodiscard]] ObjectType type() const override { return ObjectType::Button; }
    [[nodiscard]] const std::shared_ptr<Label>& label() const { return m_label; }

    void set_position(IntPosition) override;
    void set_size(IntSize) override;

    void on_hover_begin(IntPosition) override { m_hovered = true; }
    void on_hover_end() override { m_hovered = false; }
    void on_click(MouseButton mouse_button, IntPosition position) override;
    void draw() override;

private:
    [[nodiscard]] IntPosition compute_label_position();
    void reset_label();

    std::function<void(IntPosition)> m_on_click {};
    Color m_outline_color { default_palette::gray };
    Color m_background_color { default_palette::dark_gray };
    Color m_hover_color { default_palette::light_gray };
    std::shared_ptr<Label> m_label { nullptr };
    Alignment m_label_alignment { Alignment::Center };

    bool m_hovered { false };
    bool m_has_frame { true };
};

}

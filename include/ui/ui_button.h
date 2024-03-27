#pragma once

#include <functional>
#include <ui/alignment.h>
#include <ui/ui_label.h>

namespace calamus::UI {

class Button final : public Object {
public:
    Button(std::shared_ptr<Label>& label, IntPosition position, IntSize size, std::function<void(IntPosition)> on_click, Alignment label_alignment = Alignment::Center);
    ~Button() override = default;

    static std::shared_ptr<Button> create(std::shared_ptr<Label>&& label, IntPosition position, IntSize size, std::function<void(IntPosition)>&& on_click, Alignment label_alignment = Alignment::Center) {
        return std::make_shared<Button>(label, position, size, on_click, label_alignment);
    }

    [[nodiscard]] bool is_hoverable() const override { return true; }
    [[nodiscard]] bool is_clickable() const override { return true; }

    [[nodiscard]] ObjectType type() const override;

    void set_position(IntPosition) override;
    void set_size(IntSize) override;

    void on_hover_begin(IntPosition) override;
    void on_hover_end() override;
    void on_click(MouseButton, IntPosition) override;
    void draw() override;

    [[nodiscard]] const std::shared_ptr<Label>& label() const;
    [[nodiscard]] Color label_color() const noexcept;
    void set_label_color(Color);
    [[nodiscard]] std::optional<Color> label_hover_color() const noexcept;
    void set_label_hover_color(Color);

    [[nodiscard]] Color outline_color() const noexcept;
    void set_outline_color(Color);
    [[nodiscard]] Color background_color() const noexcept;
    void set_background_color(Color);
    [[nodiscard]] Color hover_color() const noexcept;
    void set_hover_color(Color);

private:
    [[nodiscard]] IntPosition compute_label_position();
    void reset_label();

    std::function<void(IntPosition)> m_on_click {};

    Color m_outline_color { default_palette::gray };
    Color m_background_color { default_palette::dark_gray };
    Color m_hover_color { default_palette::light_gray };

    std::shared_ptr<Label> m_label { nullptr };
    std::optional<Color> m_label_hover_color { default_palette::black };
    // Default, non-hovered state. NOT the same as m_label->color()!
    Color m_label_color { default_palette::white };

    Alignment m_label_alignment { Alignment::Center };

    bool m_hovered { false };
    bool m_has_frame { true };
};

}

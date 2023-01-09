#pragma once

#include <resources/fonts.h>
#include <ui/ui_object.h>

namespace calamus::UI {

class Label : public Object {
public:
    Label(const char* text, i32 size, Color color, IntPosition position = { 0, 0 });
    ~Label() override = default;

    Label(const Label&) = default;
    Label& operator=(const Label&) = default;
    Label(Label&&) noexcept = default;
    Label& operator=(Label&&) noexcept = default;

    static std::shared_ptr<Label> create(const char* text, i32 size, IntPosition position = { 0, 0 }, Color color = default_palette::black) {
        return std::make_shared<Label>(text, size, color, position);
    }

    void set_size(IntSize size) override { m_size = size; }
    [[nodiscard]] IntSize size() override;

    void set_position(IntPosition position) override { m_position = position; }
    [[nodiscard]] IntPosition position() override { return m_position; }
    [[nodiscard]] ObjectType type() const override { return ObjectType::Label; }

    [[nodiscard]] const auto* text() const noexcept { return m_text; }
    [[nodiscard]] auto font_size() const noexcept { return m_font_size; }
    [[nodiscard]] auto font_type() const noexcept { return m_font_type; }
    [[nodiscard]] auto color() const noexcept { return m_color; }

    void draw() override;

private:
    [[nodiscard]] constexpr i32 spacing() const {
        return 0; // this genuinely works best.
    }

    Resources::FontType m_font_type { Resources::FontType::Regular };
    Color m_color {};
    const char* m_text { nullptr };
    i32 m_font_size { 0 };
};

}
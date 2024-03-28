#pragma once

#include <resources/fonts.h>
#include <string>
#include <ui/ui_object.h>

namespace calamus::UI {

class Label : public Object {
public:
    Label(std::string text, i32 size, Color color, IntPosition position = { 0, 0 });
    ~Label() override = default;

    Label(const Label&) = default;
    Label& operator=(const Label&) = default;
    Label(Label&&) noexcept = default;
    Label& operator=(Label&&) noexcept = default;

    static std::shared_ptr<Label> create(std::string text, i32 size, IntPosition position = { 0, 0 }, Color color = default_palette::black) {
        return std::make_shared<Label>(std::move(text), size, color, position);
    }

    [[nodiscard]] IntSize size() override;

    [[nodiscard]] ObjectType type() const override;

    void set_text(const std::string&);
    [[nodiscard]] std::string_view text() const noexcept;

    void set_font_size(i32);
    [[nodiscard]] i32 font_size() const noexcept;

    void set_font_type(FontType);
    [[nodiscard]] FontType font_type() const noexcept;

    void set_color(Color);
    [[nodiscard]] Color color() const noexcept;

    void draw() override;

private:
    void remeasure_text();

    [[nodiscard]] constexpr i32 spacing() const {
        return 0; // this genuinely works best.
    }

    FontType m_font_type { FontType::Regular };
    Color m_color {};
    std::string m_text {};
    i32 m_font_size { 0 };
};

}
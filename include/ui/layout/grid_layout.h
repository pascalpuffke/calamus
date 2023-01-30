#pragma once

#include <ui/layout/layout.h>

namespace calamus::UI {

class GridLayout final : public Layout {
public:
    void apply(const std::vector<object_ptr>& objects) override;

    [[nodiscard]] const IntRect& get(object_ptr object) override;

    void set_row_count(i32);
    void set_row_spacing(i32);
    void set_rows_homogeneous(bool);

    void set_column_count(i32);
    void set_column_spacing(i32);
    void set_columns_homogeneous(bool);

    [[nodiscard]] auto row_count() const noexcept { return m_rows; }
    [[nodiscard]] auto row_spacing() const noexcept { return m_row_spacing; }
    [[nodiscard]] auto rows_homogeneous() const noexcept { return m_rows_homogeneous; }

    [[nodiscard]] auto column_count() const noexcept { return m_columns; }
    [[nodiscard]] auto column_spacing() const noexcept { return m_column_spacing; }
    [[nodiscard]] auto columns_homogeneous() const noexcept { return m_columns_homogeneous; }

private:
    [[nodiscard]] i32 row_spacing_per_direction() const;
    [[nodiscard]] i32 column_spacing_per_direction() const;

    i32 m_rows { 3 };
    i32 m_row_spacing { 0 };
    bool m_rows_homogeneous { true };

    i32 m_columns { 3 };
    i32 m_column_spacing { 0 };
    bool m_columns_homogeneous { true };
};

}

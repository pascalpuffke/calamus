#include <ui/layout/grid_layout.h>

namespace calamus::UI {

i32 GridLayout::row_spacing_per_direction() const { return row_spacing() / 2; }
i32 GridLayout::column_spacing_per_direction() const { return column_spacing() / 2; }

void GridLayout::apply(const std::vector<object_ptr>& objects) {
    VERIFY(rows_homogeneous() && columns_homogeneous(), "TODO: Implement non-homogeneous grid layout");

    const auto width = m_parent_rect.width / column_count();
    const auto height = m_parent_rect.height / row_count();

    auto count = 0;
    for (auto& object : objects) {
        auto x = count / row_count() * width + column_spacing_per_direction();
        auto y = count % row_count() * height + row_spacing_per_direction();

        auto rect = IntRect { x, y, width - column_spacing(), height - row_spacing() };
        object->set_rect(rect);

        ++count;
    }
}

void GridLayout::set_row_count(i32 rows) {
    m_rows = rows;
}

void GridLayout::set_row_spacing(i32 spacing) {
    m_row_spacing = spacing;
}

void GridLayout::set_rows_homogeneous(bool homogeneous) {
    m_rows_homogeneous = homogeneous;
}

void GridLayout::set_column_count(i32 columns) {
    m_columns = columns;
}

void GridLayout::set_column_spacing(i32 spacing) {
    m_column_spacing = spacing;
}

void GridLayout::set_columns_homogeneous(bool homogeneous) {
    m_columns_homogeneous = homogeneous;
}

}
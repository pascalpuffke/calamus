#include <ui/ui_button.h>
#include <util/raylib/raylib_extensions.h>

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
    const auto margins = Size { 10, 0 };
    const auto usable_size = m_size - (margins * 2);
    const auto label_size = m_label->size();
    auto label_offset = m_label->position();

    switch (m_label_alignment) {
    case Alignment::Center: {
        /**
         * [            ]
         * [   dingus   ]
         * [            ]
         */
        label_offset.x = (usable_size.width - label_size.width) / 2;
        label_offset.y = (usable_size.height - label_size.height) / 2;
    } break;
    case Alignment::CenterLeft: {
        /**
         * [            ]
         * [ dingus     ]
         * [            ]
         */
        label_offset.x = margins.width;
        label_offset.y = (usable_size.height - label_size.height) / 2;
    } break;
    case Alignment::CenterRight: {
        /**
         * [            ]
         * [     dingus ]
         * [            ]
         */
        label_offset.x = usable_size.width - label_size.width;
        label_offset.y = (usable_size.height - label_size.height) / 2;
    } break;
    case Alignment::TopCenter: {
        /**
         * [   dingus   ]
         * [            ]
         * [            ]
         */
        label_offset.x = (usable_size.width - label_size.width) / 2;
    } break;
    case Alignment::TopLeft: {
        /**
         * [ dingus     ]
         * [            ]
         * [            ]
         */
    } break;
    case Alignment::TopRight: {
        /**
         * [     dingus ]
         * [            ]
         * [            ]
         */
        label_offset.x = usable_size.width - label_size.width;
    } break;
    case Alignment::BottomCenter: {
    } break;
    case Alignment::BottomLeft: {
    } break;
    case Alignment::BottomRight: {
    } break;
    }

    return label_offset;
}

void Button::draw() {
    if (m_has_frame)
        draw_rect_with_outline(rect(), m_hovered ? m_hover_color : m_background_color, m_outline_color);
    m_label->draw();
}

void Button::on_click(MouseButton mouse_button, IntPosition position) {
    if (mouse_button == MouseButton::Left)
        m_on_click(position);
}

}

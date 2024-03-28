#pragma once

namespace calamus {

enum class TextureScaling {
    // Scale texture to fill entire width. Aspect ratio may not be preserved.
    Stretch,
    // Scale texture keeping the aspect ratio. Parts of the texture may be cut off.
    Fill,
};

enum class FontType {
    Regular,
    Monospace,
    _Count,
};

}

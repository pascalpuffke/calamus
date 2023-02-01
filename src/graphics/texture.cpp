#include <graphics/texture.h>
#include <util/raylib/raylib_wrapper.h>

namespace calamus {

Texture::Texture(u32 gl_id, IntSize size, IntSize parent_size, IntPosition offset_in_parent)
    : m_gl_id(gl_id)
    , m_size(size)
    , m_parent_size(parent_size)
    , m_offset_in_parent(offset_in_parent) {
}

Texture::~Texture() {
    // We want to delete every texture only once.
    // Textures that are just tiled offsets into an already existing texture ID should be skipped.
    if (offset() != IntPosition { 0, 0 })
        return;

    wrapper::rtextures::unload_texture(*this);
    LOG_DEBUG("Unloaded texture ID {}", id());
}

}
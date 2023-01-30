#include <graphics/texture.h>
#include <util/raylib/raylib_wrapper.h>

namespace calamus {

Texture::Texture(u32 gl_id, IntSize size)
    : m_gl_id(gl_id)
    , m_size(size) {
}

Texture::~Texture() {
    wrapper::rtextures::unload_texture(*this);
    LOG_DEBUG("Unloaded texture ID {}", m_gl_id);
}

}
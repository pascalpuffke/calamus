#include <graphics/texture.h>
#include <util/raylib/raylib_wrapper.h>

namespace calamus {

Texture::Texture(std::span<u8> image_data, IntSize size, i32 mipmaps, i32 format)
    : m_image_data(image_data)
    , m_gl_id(wrapper::rtextures::upload_texture(image_data, size, mipmaps, format))
    , m_size(size)
    , m_mipmaps(mipmaps)
    , m_format(format) {
    println("loaded texture @ {} with gl id {}, {} mipmaps, format {}, size {}", fmt::ptr(this), m_gl_id, m_mipmaps, m_format, m_size);
}

Texture::~Texture() {
    wrapper::rtextures::unload_texture(*this);
    println("unloaded texture @ {} with gl id {}", fmt::ptr(this), m_gl_id);
}

}
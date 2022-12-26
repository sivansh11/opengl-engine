#include "texture.hpp"

#include "stb_image/stb_image.hpp"

#include <assert.h>
#include <iostream>
#include <cmath>

namespace gfx {

Texture2D::Texture2D(const std::string& imagePath) {
    int width, height, nChannels;
    stbi_set_flip_vertically_on_load(true);
    uint8_t *bytes = stbi_load(imagePath.c_str(), &width, &height, &nChannels, 0);

    if (!bytes) {
        throw std::runtime_error(stbi_failure_reason());
    }
    glCreateTextures(GL_TEXTURE_2D, 1, &id);

    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);

    m_width = static_cast<uint32_t>(width);
    m_height = static_cast<uint32_t>(height); 

    GLsizei level = 1 + std::ceil(std::log2(static_cast<double>(std::max(m_width, m_height))));
    glTextureStorage2D(id, level, GL_RGBA8, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

    GLenum format = GL_RGBA;
    if (nChannels == 1) format = GL_RED;
    if (nChannels == 2) format = GL_RG;
    if (nChannels == 3) format = GL_RGB;
    if (nChannels == 4) format = GL_RGBA;

    glTextureSubImage2D(id, 0, 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), format, GL_UNSIGNED_BYTE, bytes);
    glGenerateTextureMipmap(id);

    stbi_image_free(bytes);

    // handle = glGetTextureHandleARB(id);
}

Texture2D::Texture2D(unsigned char *data, GLenum format) {
    glCreateTextures(GL_TEXTURE_2D, 1, &id);
    glTextureStorage2D(id, 1, GL_RGBA8, 1, 1);

    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureSubImage2D(id, 0, 0, 0, 1, 1, format, GL_UNSIGNED_BYTE, data);
    // glGenerateTextureMipmap(id);
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &id);
}

Texture2D::Texture2D(Texture2D&& texture) {
    id = texture.id;
    m_width = texture.m_width;
    m_height = texture.m_height;

    texture.id = {};
    texture.m_width = {};
    texture.m_height = {};
}

Texture2D& Texture2D::operator=(Texture2D&& texture) {
    id = texture.id;
    m_width = texture.m_width;
    m_height = texture.m_height;

    texture.id = {};
    texture.m_width = {};
    texture.m_height = {};

    return *this;
}

void Texture2D::bind(const std::string& name, uint32_t unit, ShaderProgram& shader) const {
    glBindTextureUnit(unit, id);
    shader.veci(name, unit);
}

} // namespace gfx

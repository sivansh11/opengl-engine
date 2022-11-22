#ifndef GFX_TEXTURE_HPP
#define GFX_TEXTURE_HPP

#include "shaders.hpp"

#include <glad/glad.h>

namespace core {

namespace gfx {

class Texture2D {
public:
    Texture2D() = default;
    Texture2D(const char *imagePath);
    ~Texture2D();
    Texture2D(Texture2D&& texture);
    Texture2D& operator=(Texture2D&& texture);

    Texture2D(Texture2D& texture) = delete;
    Texture2D& operator=(Texture2D& texture) = delete;

    void bind(const char *name, uint32_t unit, ShaderProgram& shader) const;

    const GLuint getID() const { return id; }

    Texture2D(unsigned char *data, GLenum format);

private:
    GLuint id;
    uint32_t m_width, m_height;

};

} // namespace gfx

} // namespace core

#endif
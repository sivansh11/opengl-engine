#ifndef GFX_TEXTURE_HPP
#define GFX_TEXTURE_HPP

#include "shaders.hpp"

#include <glad/glad.h>

#include <string>

namespace gfx {

class Texture2D {
public:
    Texture2D() = default;
    Texture2D(const std::string& imagePath);
    ~Texture2D();
    Texture2D(Texture2D&& texture);
    Texture2D& operator=(Texture2D&& texture);

    Texture2D(Texture2D& texture) = delete;
    Texture2D& operator=(Texture2D& texture) = delete;

    void bind(const std::string& name, uint32_t unit, ShaderProgram& shader) const;

    const GLuint getID() const { return id; }

    Texture2D(unsigned char *data, GLenum format);

    Texture2D(GLuint id);

private:
    GLuint id;
    bool m_owned = true;
    GLuint64 handle;
    uint32_t m_width, m_height;

};

} // namespace gfx

#endif
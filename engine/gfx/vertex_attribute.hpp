#ifndef GFX_VERTEX_ATTRIBUTE_HPP
#define GFX_VERTEX_ATTRIBUTE_HPP

#include <glad/glad.h>

#include "buffer.hpp"

namespace gfx   {

class VertexAttribute {
public:
    VertexAttribute() {
        glCreateVertexArrays(1, &id);
    }

    ~VertexAttribute() {
        glDeleteVertexArrays(1, &id);
    }

    VertexAttribute(VertexAttribute&& vertexAttribute) {
        id = vertexAttribute.id;

        vertexAttribute.id = {};
    }

    VertexAttribute& operator=(VertexAttribute&& vertexAttribute) {
        id = vertexAttribute.id;

        vertexAttribute.id = {};

        return *this;
    }
    
    VertexAttribute(VertexAttribute& vertexAttribute) = delete;
    VertexAttribute& operator=(VertexAttribute& vertexAttribute) = delete;

    void attributeLocation(uint32_t location, uint32_t size, uint32_t relativeOffset) {
        glEnableVertexArrayAttrib(id, static_cast<GLuint>(location));
        glVertexArrayAttribBinding(id, static_cast<GLuint>(location), 0);
        glVertexArrayAttribFormat(id, static_cast<GLuint>(location), static_cast<GLuint>(size), GL_FLOAT, GL_FALSE, static_cast<GLuint>(relativeOffset));
    }

    template <typename T>
    void bindVertexBuffer(const Buffer& buffer) {
        glVertexArrayVertexBuffer(id, 0, buffer.getID(), 0, sizeof(T));
    }

    void bindElementBuffer(const Buffer& buffer) {
        glVertexArrayElementBuffer(id, buffer.getID());
    }

    void bind() {
        glBindVertexArray(id);
    }

private:
    GLuint id;
};

} // namespace gfx 

#endif
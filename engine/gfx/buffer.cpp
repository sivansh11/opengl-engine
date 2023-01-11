#include "buffer.hpp"

#include <cstring>
#include <assert.h>

namespace gfx {

Buffer::Buffer(uint32_t bytes, Buffer::Useage type) : m_bytes(bytes), m_type(type) {
    glCreateBuffers(1, &id);
    resize(m_bytes);
}

Buffer::Buffer(Buffer::Useage type) : m_type(type) {
    glCreateBuffers(1, &id);
}

Buffer::Buffer(Buffer&& buffer) {
    id = buffer.id;
    m_bytes = buffer.m_bytes;
    m_type = buffer.m_type;

    buffer.id = 0;
    buffer.m_bytes = 0;
    buffer.m_type = {};
}

Buffer& Buffer::operator=(Buffer&& buffer) {
    id = buffer.id;
    m_bytes = buffer.m_bytes;
    m_type = buffer.m_type;

    buffer.id = 0;
    buffer.m_bytes = 0;
    buffer.m_type = {};
    
    return *this;
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &id);
}


void Buffer::push(const void *sourceData, uint32_t offset, uint32_t bytes) {
    assert(sourceData != nullptr);
    GLenum drawType;
    switch (m_type) {
        case Buffer::Useage::eSTATIC_DRAW:
            drawType = GL_STATIC_DRAW;
            break;
        case Buffer::Useage::eDYNAMIC_DRAW:
            drawType = GL_DYNAMIC_DRAW;
            break;
        
        default:
            throw std::runtime_error("Not implemented!");
            break;
    }

    if (offset == 0 && bytes == std::numeric_limits<uint32_t>::max()) {
        glNamedBufferData(id, static_cast<GLsizeiptr>(m_bytes), sourceData, drawType);
    } else {
        glNamedBufferSubData(id, offset, bytes, sourceData);
    }
}

void Buffer::pull(void *destinationBuffer, uint32_t offset, uint32_t bytes) const {
    assert(destinationBuffer != nullptr);
    if (offset == 0 && bytes == std::numeric_limits<uint32_t>::max()) {
        void *mapped = glMapNamedBufferRange(id, 0, static_cast<GLsizeiptr>(m_bytes), GL_MAP_READ_BIT);
        std::memcpy(destinationBuffer, mapped, static_cast<uint32_t>(m_bytes));
        glUnmapNamedBuffer(id);
    } else {
        void *mapped = glMapNamedBufferRange(id, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(bytes), GL_MAP_READ_BIT);
        reinterpret_cast<char *&>(mapped) += offset;
        std::memcpy(destinationBuffer, mapped, static_cast<uint32_t>(m_bytes));        
        glUnmapNamedBuffer(id);
    }
}

// void *Buffer::map(BufferOperation operation, uint32_t offset, uint32_t bytes) {
//     GLenum glOperation{};
//     switch (operation) {
//         case BufferOperation::READ_OPERATION:
//             glOperation = GL_MAP_READ_BIT;
//             break;
//         case BufferOperation::WRITE_OPERATION:
//             glOperation = GL_MAP_WRITE_BIT;
//             break;
//         case BufferOperation::READ_WRITE_OPERATION:
//             glOperation = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
        
//         default:
//             break;
//     }
//     if ((offset == 0) && (bytes == std::numeric_limits<uint32_t>::max())) {
//         return glMapNamedBufferRange(id, 0, static_cast<GLsizeiptr>(m_bytes), glOperation);
//     } else {
//         return glMapNamedBufferRange(id, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(bytes), glOperation); 
//     }
// }

// void Buffer::unmap() const {
//     glUnmapNamedBuffer(id);
// }

void Buffer::bind(uint32_t binding) const {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, static_cast<GLuint>(binding), id);
}

void Buffer::resize(uint32_t bytes) {
    m_bytes = bytes;
    switch (m_type) {
       case Buffer::Useage::eSTATIC_DRAW:
            glNamedBufferData(id, m_bytes, nullptr, GL_STATIC_DRAW);
            break;
        case Buffer::Useage::eDYNAMIC_DRAW:
            glNamedBufferData(id, m_bytes, nullptr, GL_DYNAMIC_DRAW);
            break;
            
        default:
            throw std::runtime_error("Not implemented!");
            break;
    }
}
} // namespace gfx

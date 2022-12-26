#ifndef GFX_BUFFER_HPP
#define GFX_BUFFER_HPP

#include <glad/glad.h>

#include <iostream>
#include <limits>

namespace gfx {

enum BufferType {
    STATIC_BUFFER,
    DYNAMIC_BUFFER
};

enum BufferOperation {
    READ_OPERATION,
    WRITE_OPERATION,
    READ_WRITE_OPERATION,
};

class Buffer {
public:
    Buffer() = default;
    Buffer(uint32_t bytes, BufferType type = BufferType::STATIC_BUFFER);
    Buffer(BufferType type);

    Buffer(Buffer&& buffer);
    Buffer& operator=(Buffer&& buffer);
    ~Buffer();
    
    Buffer(Buffer&) = delete;
    Buffer& operator=(Buffer& Buffer) = delete;

    void push(const void *sourceData);
    void pull(void *destinationBuffer, uint32_t offset = 0, uint32_t bytes = std::numeric_limits<uint32_t>::max()) const;

    void * map(BufferOperation operation, uint32_t offset = 0, uint32_t bytes = std::numeric_limits<uint32_t>::max());
    void unmap() const;

    void bind(uint32_t binding)const;
    void resize(uint32_t bytes);

    uint32_t capacity() const { return m_bytes; }
    GLuint getID() const { return id; }

private:
    GLuint id;
    uint32_t m_bytes;
    BufferType m_type;

};

} // namespace gfx

#endif
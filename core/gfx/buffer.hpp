#ifndef GFX_BUFFER_HPP
#define GFX_BUFFER_HPP

#include <glad/glad.h>

#include <iostream>
#include <limits>

namespace core {

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

/**
 * @brief create memory on the gpu
 * 
 */
class Buffer {
public:
    Buffer() = default;
    /**
     * @brief Construct a new Buffer object
     * 
     * @param bytes 
     * @param type 
     */
    Buffer(uint32_t bytes, BufferType type = BufferType::STATIC_BUFFER);
    /**
     * @brief Construct a new Buffer object of size 0 
     * 
     * @param type 
     */
    Buffer(BufferType type);

    // NOTE: you can move ownership 

    Buffer(Buffer&& buffer);
    Buffer& operator=(Buffer&& buffer);
    ~Buffer();
    
    // NOTE: you cannot share a buffer
    Buffer(Buffer&) = delete;
    Buffer& operator=(Buffer& Buffer) = delete;

    /**
     * @brief push memory from CPU -> GPU, sourceData should not be null
     * 
     * @param sourceData 
     * @param offset 
     * @param bytes 
     */
    void push(const void *sourceData);
    /**
     * @brief pull memory from the GPU -> CPU, destinationBuffer should be a valid pointer with the required size 
     * 
     * @param destinationBuffer 
     * @param offset 
     * @param bytes 
     */
    void pull(void *destinationBuffer, uint32_t offset = 0, uint32_t bytes = std::numeric_limits<uint32_t>::max()) const;

    /**
     * @brief create a user side pointer for pointing to a memory buffer
     * 
     * @param operation 
     * @param offset 
     * @param bytes 
     * @return void* const 
     */
    void * map(BufferOperation operation, uint32_t offset = 0, uint32_t bytes = std::numeric_limits<uint32_t>::max());
    /**
     * @brief frees the cpu side memory allocated with map 
     * 
     */
    void unmap() const;

    /**
     * @brief used in case of SSBO, binding is the binding index used by opengl
     * 
     * @param binding 
     */
    void bind(uint32_t binding);
    /**
     * @brief resize buffer incase more memory is required
     * 
     * @param bytes 
     */
    void resize(uint32_t bytes);

    /**
     * @brief returns the current capacity of the buffer
     * 
     * @return uint32_t 
     */
    uint32_t capacity() const { return m_bytes; }
    /**
     * @brief returns the underlying handle used by opengl
     * 
     * @return GLuint 
     */
    GLuint getID() const { return id; }

private:
    GLuint id;
    uint32_t m_bytes;
    BufferType m_type;

};

} // namespace gfx

} // namespace core

#endif
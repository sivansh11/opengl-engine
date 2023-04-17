#ifndef GFX_BUFFER_HPP
#define GFX_BUFFER_HPP

#include "type.hpp"

#include <glad/glad.h>

#include <iostream>
#include <limits>

namespace gfx {

class Buffer {
public:
    enum class Useage : GLenum {
        eSTATIC_DRAW = GL_STATIC_DRAW,
        eSTREAM_DRAW = GL_STREAM_DRAW,
        eDYNAMIC_DRAW = GL_DYNAMIC_DRAW,
    };

    class MapBit : public BaseFlag<GLbitfield> {
    public:
        MapBit() {}
        MapBit(GLenum flag) : BaseFlag(flag) {}
        operator GLenum() {
            return flags;
        }

        constexpr static GLbitfield eREAD = GL_MAP_READ_BIT;
        constexpr static GLbitfield eWRITE = GL_MAP_WRITE_BIT;
    };

    Buffer() = default;
    Buffer(uint32_t bytes, Useage type = Useage::eSTATIC_DRAW, const std::string& debugName = "default");
    Buffer(Useage type, const std::string& debugName = "default");

    Buffer(Buffer&& buffer);
    Buffer& operator=(Buffer&& buffer);
    ~Buffer();
    
    Buffer(Buffer&) = delete;
    Buffer& operator=(Buffer& Buffer) = delete;

    void push(const void *sourceData, uint32_t offset = 0, uint32_t bytes = std::numeric_limits<uint32_t>::max());
    void pull(void *destinationBuffer, uint32_t offset = 0, uint32_t bytes = std::numeric_limits<uint32_t>::max()) const;

    // void * map(MapBit operation, uint32_t offset = 0, uint32_t bytes = std::numeric_limits<uint32_t>::max());
    // void unmap() const;

    void bind(uint32_t binding)const;
    void resize(uint32_t bytes);

    uint32_t capacity() const { return m_bytes; }
    GLuint getID() const { return id; }

private:
    GLuint id;
    uint32_t m_bytes;
    Useage m_type;

};

} // namespace gfx

#endif
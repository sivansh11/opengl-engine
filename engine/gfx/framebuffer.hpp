#ifndef GFX_FRAMEBUFFER_HPP
#define GFX_FRAMEBUFFER_HPP

#include "texture.hpp"

#include <glad/glad.h>

#include <map>
#include <vector>
#include <array>

namespace gfx {

struct FrameBufferVertex {
    float pos[2];
    float uv[2];
};

const std::vector<FrameBufferVertex> frameBufferQuadVertices {
    FrameBufferVertex{{-1,  1}, {0, 1}},
    FrameBufferVertex{{-1, -1}, {0, 0}},
    FrameBufferVertex{{ 1,  1}, {1, 1}},
    FrameBufferVertex{{ 1, -1}, {1, 0}},
};

class FrameBuffer {
public:
    class BufferBit : public BaseFlag<GLbitfield> {
    public:
        BufferBit() = default;
        BufferBit(GLbitfield flag) : BaseFlag(flag) {}
        operator GLbitfield() {
            return flags;
        }
        constexpr static GLbitfield eCOLOR = GL_COLOR_BUFFER_BIT;
        constexpr static GLbitfield eDEPTH = GL_DEPTH_BUFFER_BIT;
        constexpr static GLbitfield eSTENCIL = GL_STENCIL_BUFFER_BIT;
    };

    enum class Attachment : GLenum {
        eCOLOR0 = GL_COLOR_ATTACHMENT0,
        eCOLOR1 = GL_COLOR_ATTACHMENT1,
        eCOLOR2 = GL_COLOR_ATTACHMENT2,
        eCOLOR3 = GL_COLOR_ATTACHMENT3,
        eCOLOR4 = GL_COLOR_ATTACHMENT4,
        eCOLOR5 = GL_COLOR_ATTACHMENT5,
        eCOLOR6 = GL_COLOR_ATTACHMENT6,
        eCOLOR7 = GL_COLOR_ATTACHMENT7,
        eCOLOR8 = GL_COLOR_ATTACHMENT8,
        eCOLOR9 = GL_COLOR_ATTACHMENT9,
        eCOLOR10 = GL_COLOR_ATTACHMENT10,
        eCOLOR11 = GL_COLOR_ATTACHMENT11,
        eCOLOR12 = GL_COLOR_ATTACHMENT12,
        eCOLOR13 = GL_COLOR_ATTACHMENT13,
        eCOLOR14 = GL_COLOR_ATTACHMENT14,
        eCOLOR15 = GL_COLOR_ATTACHMENT15,
        eCOLOR16 = GL_COLOR_ATTACHMENT16,
        eCOLOR17 = GL_COLOR_ATTACHMENT17,
        eCOLOR18 = GL_COLOR_ATTACHMENT18,
        eDEPTH = GL_DEPTH_ATTACHMENT,
        eDEPTH_STENCIL = GL_DEPTH_STENCIL_ATTACHMENT,
    };

    struct TextureMipLevel {
        std::shared_ptr<Texture> texture;
        uint32_t mipLevel;
    };
    struct Builder {
        std::map<Attachment, TextureMipLevel> textures;
        uint32_t width, height;

        Builder(uint32_t width, uint32_t height);

        Builder& addAttachment(gfx::Texture::Builder& builder, gfx::Texture::Type type, gfx::Texture::InternalFormat internalFormat, Attachment Attachment, uint32_t mipLevel = 0);

        // Builder& addAttachment(std::shared_ptr<Texture> texture, Attachment attachment, uint32_t mipLevel = 0);
        // Builder& addAttachment(Texture::Builder& builder, Texture::Type type, Texture::CreateInfo& info, Attachment attachment, uint32_t mipLevel = 0);
        FrameBuffer build();
    };

    FrameBuffer() = default;
    FrameBuffer(GLuint id, std::map<Attachment, TextureMipLevel>& textures, uint32_t width, uint32_t height);
    ~FrameBuffer();

    FrameBuffer(const FrameBuffer& other) = delete;
    FrameBuffer& operator=(const FrameBuffer& other) = delete;

    FrameBuffer(FrameBuffer&& other);
    FrameBuffer& operator=(FrameBuffer&& other);

    void setClearColor(float r, float g, float b, float a);
    void setClearDepth(float d);
    void setClearStencil(int s);
    void clear(BufferBit bits);

    GLuint getID() { return id; }
    std::shared_ptr<Texture> getTexture(Attachment attachment) const; 

    void bind();
    void unbind();

    void invalidate(uint32_t width, uint32_t height);

private:
    GLuint id;
    uint32_t width, height;
    std::map<Attachment, TextureMipLevel> textures;
    std::array<float, 4> clearColor;
    int clearStencil;
    float clearDepth = 1;
};

} // namespace gfx

#endif
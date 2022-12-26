#ifndef GFX_FRAMEBUFFER_HPP
#define GFX_FRAMEBUFFER_HPP

#include <glad/glad.h>

#include <map>
#include <vector>

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

struct TextureFormatType {
    GLenum format;
    GLenum type;
};

struct FrameBufferAttachmentFormat {
    GLenum attachment;
    TextureFormatType formatType;
};

struct FrameBufferInfo {
    std::vector<FrameBufferAttachmentFormat> attachments;
    uint32_t width, height;
};


class FrameBuffer {
public:
    FrameBuffer() = default;
    FrameBuffer(FrameBufferInfo frameBufferInfo);
    ~FrameBuffer();

    FrameBuffer(FrameBuffer&& frameBuffer);
    FrameBuffer& operator=(FrameBuffer&& frameBuffer);

    FrameBuffer(FrameBuffer& framebuffer) = delete;
    FrameBuffer& operator=(FrameBuffer& frameBuffer) = delete;

    void bind();
    void unbind();

    GLuint getTextureID(GLenum type) const;

    void invalidate(uint32_t newWidth, uint32_t newHeight);

    float getAspect() const { return float(m_frameBufferInfo.width) / float(m_frameBufferInfo.height); }

    uint32_t getWidth() const { return m_frameBufferInfo.width; }
    uint32_t getHeight() const { return m_frameBufferInfo.height; }

    GLuint getID() const { return id; }

private:
    void create();
    void destroy();

private:
    GLuint id = 0;
    FrameBufferInfo m_frameBufferInfo;
    std::map<GLenum, GLuint> m_frameBufferAttachments;
};

} // namespace gfx

#endif
#include "framebuffer.hpp"

#include <iostream>
#include <assert.h>

namespace core {

namespace gfx {

FrameBuffer::FrameBuffer(FrameBufferInfo frameBufferInfo) : m_frameBufferInfo(frameBufferInfo) {
    create();
}

FrameBuffer::FrameBuffer(FrameBuffer&& frameBuffer) {
    id = frameBuffer.id;
    m_frameBufferInfo = std::move(frameBuffer.m_frameBufferInfo);
    m_frameBufferAttachments = std::move(frameBuffer.m_frameBufferAttachments);

    frameBuffer.id = 0;
    frameBuffer.m_frameBufferInfo = {};
    frameBuffer.m_frameBufferAttachments = {};
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& frameBuffer) {
    id = frameBuffer.id;
    m_frameBufferInfo = std::move(frameBuffer.m_frameBufferInfo);
    m_frameBufferAttachments = std::move(frameBuffer.m_frameBufferAttachments);

    frameBuffer.id = 0;
    frameBuffer.m_frameBufferInfo = {};
    frameBuffer.m_frameBufferAttachments = {};
    
    return *this;
}

void FrameBuffer::create() {
    glCreateFramebuffers(1, &id);

    std::vector<GLenum> attachments;

    for (const auto& attachmentTypeFormat : m_frameBufferInfo.attachments) {

        if (attachmentTypeFormat.attachment >= GL_COLOR_ATTACHMENT0 && attachmentTypeFormat.attachment <= GL_COLOR_ATTACHMENT31) {
            GLuint tex;
            glCreateTextures(GL_TEXTURE_2D, 1, &tex);
            glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexImage2D(GL_TEXTURE_2D, 0, attachmentTypeFormat.formatType.format, static_cast<GLsizei>(m_frameBufferInfo.width), static_cast<GLsizei>(m_frameBufferInfo.height), 0, GL_RGBA, attachmentTypeFormat.formatType.type, nullptr);
            glBindTexture(GL_TEXTURE_2D, 0);
            glNamedFramebufferTexture(id, attachmentTypeFormat.attachment, tex, 0);
            attachments.push_back(attachmentTypeFormat.attachment);
            m_frameBufferAttachments.emplace(attachmentTypeFormat.attachment, tex);
        } else {
            GLuint ren;
            glCreateRenderbuffers(1, &ren);
            glNamedRenderbufferStorage(ren, GL_DEPTH_COMPONENT, static_cast<GLsizei>(m_frameBufferInfo.width), static_cast<GLsizei>(m_frameBufferInfo.height));
            glNamedFramebufferRenderbuffer(id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ren);
            m_frameBufferAttachments.emplace(GL_DEPTH_ATTACHMENT, ren);
        }
    }

    glNamedFramebufferDrawBuffers(id, attachments.size(), attachments.data());

    auto fboStatus = glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer error: " << fboStatus << "\n";
        throw std::runtime_error("");
    }
}

void FrameBuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void FrameBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FrameBuffer::getTextureID(GLenum type) {
    assert(m_frameBufferAttachments.contains(type));
    return m_frameBufferAttachments.at(type);
}

void FrameBuffer::destroy() {
    for (auto& kv : m_frameBufferAttachments) {
        if (kv.first >= GL_COLOR_ATTACHMENT0 && kv.first <= GL_COLOR_ATTACHMENT31) {
            glDeleteTextures(1, &kv.second);
        } else {
            glDeleteRenderbuffers(1, &kv.second);
        }
    }
    glDeleteFramebuffers(1, &id);   
    m_frameBufferAttachments.clear();
}

FrameBuffer::~FrameBuffer() {
    destroy();
}

void FrameBuffer::invalidate(uint32_t newWidth, uint32_t newHeight) {
    if (newWidth != m_frameBufferInfo.width || newHeight != m_frameBufferInfo.height) {
        destroy();
        m_frameBufferInfo.width = newWidth, m_frameBufferInfo.height = newHeight;
        create();
    }
}

} // namespace gfx

} // namespace core

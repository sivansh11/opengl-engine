#include "framebuffer.hpp"

#include <iostream>
#include <assert.h>

namespace gfx {

FrameBuffer::Builder::Builder(uint32_t width, uint32_t height) : width(width), height(height) {

}

FrameBuffer::FrameBuffer(FrameBuffer&& other) {
    id = other.id;
    width = other.width;
    height = other.height;
    textures = other.textures;
    clearColor = other.clearColor;
    clearStencil = other.clearStencil;
    clearDepth = other.clearDepth;

    other.id = 0;
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) {
    id = other.id;
    width = other.width;
    height = other.height;
    textures = other.textures;
    clearColor = other.clearColor;
    clearStencil = other.clearStencil;
    clearDepth = other.clearDepth;

    other.id = 0;
    return *this;
}

FrameBuffer::Builder& FrameBuffer::Builder::addAttachment(const gfx::Texture::Builder& builder, gfx::Texture::Type type, gfx::Texture::InternalFormat internalFormat, FrameBuffer::Attachment attachment, uint32_t mipLevel) {
    assert(!textures.contains(attachment));
    auto tex = builder.build(type);
    gfx::Texture::CreateInfo info;
    info.width = width;
    info.height = height;
    info.genMipMap = false;
    info.internalFormat = internalFormat;
    tex->create(info);
    textures[attachment] = {tex, mipLevel};
    return *this;
}

// FrameBuffer::Builder& FrameBuffer::Builder::addAttachment(std::shared_ptr<Texture> texture, Attachment attachment, uint32_t mipLevel) {
//     assert(!textures.contains(attachment));
//     textures[attachment] = {texture, mipLevel};
//     return *this;
// }

// FrameBuffer::Builder& FrameBuffer::Builder::addAttachment(Texture::Builder& builder, Texture::Type type, Texture::CreateInfo& info, Attachment attachment, uint32_t mipLevel) {
//     assert(!textures.contains(attachment));
//     auto tex = builder.build(type);
//     tex->create(info);
//     textures[attachment] = {tex, mipLevel};
//     return *this;
// }

FrameBuffer FrameBuffer::Builder::build() {
    GLuint id;
    glCreateFramebuffers(1, &id);

    std::vector<GLenum> attachments;

    for (auto& kv : textures) {
        auto& attachment = kv.first;
        auto& textureMipLevel = kv.second;
        auto& texture = textureMipLevel.texture;
        auto& mipLevel = textureMipLevel.mipLevel;

        glNamedFramebufferTexture(id, static_cast<GLenum>(attachment), texture->getID(), mipLevel);

        if (attachment != Attachment::eDEPTH && attachment != Attachment::eDEPTH_STENCIL) {
            attachments.push_back(static_cast<GLenum>(attachment));
        }

    }

    glNamedFramebufferDrawBuffers(id, attachments.size(), attachments.data());

    auto fboStatus = glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer error: " << fboStatus << "\n";
        throw std::runtime_error("");
    }

    return {id, textures, width, height};
}

FrameBuffer::FrameBuffer(GLuint id, std::map<Attachment, TextureMipLevel>& textures, uint32_t width, uint32_t height) : id(id), textures(textures), width(width), height(height) {
    
}
FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &id);
}

std::shared_ptr<Texture> FrameBuffer::getTexture(FrameBuffer::Attachment attachment) const {
    assert(textures.contains(attachment));
    return textures.at(attachment).texture;
}

// TODO: fix bug, create new framebuffer along with textures
void FrameBuffer::invalidate(uint32_t width, uint32_t height) {
    if (width != FrameBuffer::width || height != FrameBuffer::height) {
        FrameBuffer::width = width;
        FrameBuffer::height = height;

        for (auto& kv : textures) {
            auto& textureMip = kv.second;
            auto& texture = textureMip.texture;
            auto& attachment = kv.first;
            auto& mipLevel = textureMip.mipLevel;

            auto info = texture->getInfo();
            info.width = width;
            info.height = height;
            texture->resize(info);

            glNamedFramebufferTexture(id, static_cast<GLenum>(attachment), texture->getID(), mipLevel);
        }
    }

    auto fboStatus = glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer error: " << fboStatus << "\n";
        throw std::runtime_error("");
    }
    
    // for (auto& kv : textures) {
    //     auto& texture = kv.second;

    //     auto info = texture->getInfo();
    //     if (info.width != width || info.height != height) {
    //         info.width = width;
    //         info.height = height;
    //         texture->resize(info);
    //     }
    // }
}

void FrameBuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, width, height);
}

void FrameBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::clear(BufferBit bits) {
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClearDepthf(clearDepth);
    glClearStencil(clearStencil);
    glClear(bits);
}

void FrameBuffer::setClearColor(float r, float g, float b, float a) {
    clearColor[0] = r;
    clearColor[1] = g;
    clearColor[2] = b;
    clearColor[3] = a;
}

void FrameBuffer::setClearDepth(float d) {
    clearDepth = d;
}

void FrameBuffer::setClearStencil(int s) {
    clearStencil = s;
}

} // namespace gfx

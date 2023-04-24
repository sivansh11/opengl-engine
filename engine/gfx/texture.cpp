#include "texture.hpp"

#include "stb_image/stb_image.hpp"

#include <assert.h>
#include <iostream>
#include <cmath>

namespace gfx {

Texture::Builder::Builder() {
    wrapS = Wrap::eREPEAT;
    wrapT = Wrap::eREPEAT;
    wrapR = Wrap::eREPEAT;

    minFilter = MinFilter::eLINEAR;
    magFilter = MagFilter::eLINEAR;

    borderColor[0] = 0;
    borderColor[1] = 0;
    borderColor[2] = 0;
    borderColor[3] = 0;

    swizzles[0] = Swizzle::eR;
    swizzles[1] = Swizzle::eG;
    swizzles[2] = Swizzle::eB;
    swizzles[3] = Swizzle::eA;

    compareFunc = CompareFunc::eLEQUAL;
    compareMode = CompareMode::eCOMPARE_REF_TO_TEXTURE;
}

Texture::Builder& Texture::Builder::setMinFilter(MinFilter minFilter) {
    Texture::Builder::minFilter = minFilter;
    return *this;
}

Texture::Builder& Texture::Builder::setMagFilter(MagFilter magFilter) {
    Texture::Builder::magFilter = magFilter;
    return *this;
}

Texture::Builder& Texture::Builder::setWrapS(Wrap wrap) {
    Texture::Builder::wrapS = wrap;
    return *this;
}

Texture::Builder& Texture::Builder::setWrapT(Wrap wrap) {
    Texture::Builder::wrapT = wrap;
    return *this;
}

Texture::Builder& Texture::Builder::setWrapR(Wrap wrap) {
    Texture::Builder::wrapR = wrap;
    return *this;
}

Texture::Builder& Texture::Builder::setSwizzleR(Swizzle swizzle) {
    swizzles[0] = swizzle;   
    return *this;
}

Texture::Builder& Texture::Builder::setSwizzleG(Swizzle swizzle) {
    swizzles[1] = swizzle;   
    return *this;
}

Texture::Builder& Texture::Builder::setSwizzleB(Swizzle swizzle) {
    swizzles[2] = swizzle;   
    return *this;
}

Texture::Builder& Texture::Builder::setSwizzleA(Swizzle swizzle) {
    swizzles[3] = swizzle;   
    return *this;
}

Texture::Builder& Texture::Builder::setBorderColor(float r, float g, float b, float a) {
    Texture::Builder::borderColor[0] = r;
    Texture::Builder::borderColor[1] = g;
    Texture::Builder::borderColor[2] = b;
    Texture::Builder::borderColor[3] = a;
    return *this;
}

Texture::Builder& Texture::Builder::setBorderColor(float d) {
    Texture::Builder::borderColor[0] = d;
    return *this;
}

Texture::Builder& Texture::Builder::setCompareFunc(CompareFunc compareFunc) {
    Texture::Builder::compareFunc = compareFunc;
    return *this;
}

Texture::Builder& Texture::Builder::setCompareMode(CompareMode compareMode) {
    Texture::Builder::compareMode = compareMode;
    return *this;
}

std::shared_ptr<Texture> Texture::Builder::build(Type type, const std::string& debugName) const {
    auto id = getNewID(type);
    glObjectLabel(GL_TEXTURE, id, -1, debugName.c_str());
    return std::make_unique<Texture>(id, type, *this);
}

GLuint Texture::Builder::getNewID(Type type) const{
    GLuint id;

    auto minFilter_ = minFilter;
    auto magFilter_ = magFilter;

    glCreateTextures(static_cast<GLenum>(type), 1, &id);
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, minFilter_);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, magFilter_);
    glTextureParameteri(id, GL_TEXTURE_WRAP_R, static_cast<GLenum>(wrapR));
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, static_cast<GLenum>(wrapS));
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, static_cast<GLenum>(wrapT));
    glTextureParameteri(id, GL_TEXTURE_COMPARE_FUNC, static_cast<GLint>(compareFunc));
    glTextureParameteri(id, GL_TEXTURE_COMPARE_MODE, static_cast<GLint>(compareMode));

    glTextureParameteriv(id, GL_TEXTURE_SWIZZLE_RGBA, reinterpret_cast<const GLint*>(swizzles));

    glTextureParameterfv(id, GL_TEXTURE_BORDER_COLOR, borderColor);

    return id;
}

Texture::Texture(GLuint id, Type type, Builder builder) : id(id), type(type), builder(builder) {}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void Texture::changeMinFilter(MinFilter minFilter) {
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, minFilter);
}

void Texture::changeMagFilter(MagFilter magFilter) {
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::changeWrapS(Wrap wrap) {
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, static_cast<GLenum>(wrap));
}

void Texture::changeWrapT(Wrap wrap) {
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, static_cast<GLenum>(wrap));
}

void Texture::changeWrapR(Wrap wrap) {
    glTextureParameteri(id, GL_TEXTURE_WRAP_R, static_cast<GLenum>(wrap));
}

// void Texture::changeSwizzleR(Swizzle swizzle) {

// }

// void Texture::changeSwizzleG(Swizzle swizzle) {

// }

// void Texture::changeSwizzleB(Swizzle swizzle) {

// }

// void Texture::changeSwizzleA(Swizzle swizzle) {

// }

// void Texture::changeCompareFunc(CompareFunc compareFunc) {

// }

// void Texture::changeCompareMode(CompareMode compareMode) {

// }


void Texture::bind(const std::string& name, uint32_t unit, ShaderProgram& shader) const {
    glBindTextureUnit(unit, id);
    shader.veci(name, unit);
}

void Texture::bindImage(const std::string& name, uint32_t unit, uint32_t level, ShaderProgram& shader) const {
    glBindImageTexture(unit, id, level, GL_TRUE, 0, GL_WRITE_ONLY, static_cast<GLenum>(info.internalFormat));
    shader.veci(name, unit);
}

void Texture::create(const CreateInfo& info) {
    Texture::info = info;
    GLsizei targetLevel = 1;
    switch (type) {
        case Type::e2D: {
                if (info.genMipMap) targetLevel = 1 + std::floor(std::log2(static_cast<double>(std::max(info.width, info.height))));
                glTextureStorage2D(id, targetLevel, static_cast<GLenum>(info.internalFormat), info.width, info.height);
        }
        break; 
        case Type::e3D: {
            if (info.genMipMap) targetLevel = 1 + std::floor(std::log2(static_cast<double>(std::max(std::max(info.width, info.height), info.depth))));
            glTextureStorage3D(id, targetLevel, static_cast<GLenum>(info.internalFormat), info.width, info.height, info.depth);
        }
        break;
        
        default:
            std::runtime_error("Unknown type");
            break;
    }
}

void Texture::upload(const UploadInfo& uploadInfo) {
    switch (type) {
        case Type::e2D: {
            glTextureSubImage2D(id, uploadInfo.level, uploadInfo.xoffset, uploadInfo.yoffset, info.width, info.height, static_cast<GLenum>(uploadInfo.format), static_cast<GLenum>(uploadInfo.dataType), uploadInfo.data);
        }
        break; 
        case Type::e3D: {
            glTextureSubImage3D(id, uploadInfo.level, uploadInfo.xoffset, uploadInfo.yoffset, uploadInfo.zoffset, info.width, info.height, info.depth, static_cast<GLenum>(uploadInfo.format), static_cast<GLenum>(uploadInfo.dataType), uploadInfo.data);
        }
        break;
        
        default:
        std::runtime_error("Unknown type");
        break;
    }

    if (info.genMipMap) {
        glGenerateTextureMipmap(id);
    }
}

void Texture::genMipMaps() {
    glGenerateTextureMipmap(id);
}

void Texture::loadImage(std::filesystem::path path) {
    int width, height, nChannels;
    stbi_set_flip_vertically_on_load(true);
    uint8_t *bytes = stbi_load(path.string().c_str(), &width, &height, &nChannels, 0);

    if (!bytes) {

        throw std::runtime_error(stbi_failure_reason());
    }

    CreateInfo createInfo{};
    createInfo.internalFormat = InternalFormat::eRGBA8;
    createInfo.width = width;
    createInfo.height = height;
    createInfo.genMipMap = true;

    UploadInfo uploadInfo{};
    uploadInfo.data = bytes;
    uploadInfo.dataType = DataType::eUNSIGNED_BYTE;
    uploadInfo.format = Format::eRGBA;
    uploadInfo.level = 0;
    if (nChannels == 1) uploadInfo.format = Format::eRED;
    if (nChannels == 2) uploadInfo.format = Format::eRG;
    if (nChannels == 3) uploadInfo.format = Format::eRGB;
    if (nChannels == 4) uploadInfo.format = Format::eRGBA;

    create(createInfo);
    upload(uploadInfo);

    stbi_image_free(bytes);
}

void Texture::resize(Texture::CreateInfo& info) {
    // if (info.data) info.data = nullptr;
    this->~Texture();
    id = builder.getNewID(type);
    create(info);
}

CubeMap::CubeMap(const std::string& imagePath) {
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &id);

    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    std::vector<std::string> names = {
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg"
    };
    int width, height, nChannels;
    bool once = false;
    for (int i = 0; i < 6; i++) {
        uint8_t *bytes = stbi_load((imagePath + names[i]).c_str(), &width, &height, &nChannels, 3);
        if (!once) {
            glTextureStorage2D(id, 1, GL_RGBA8, width, height);
            once = true;
        }
        
        if (!bytes) {
            throw std::runtime_error(stbi_failure_reason());
        }
        glTextureSubImage3D(id, 0, 0, 0, i, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, bytes);
    }
}


CubeMap::~CubeMap() {
    glDeleteTextures(1, &id);
}

CubeMap::CubeMap(CubeMap&& cubeMap) {
    id = cubeMap.id;
    cubeMap.id = {};
}

CubeMap& CubeMap::operator=(CubeMap&& cubeMap) {
    id = cubeMap.id;
    cubeMap.id = {};

    return *this;
}

void CubeMap::bind(const std::string& name, uint32_t unit, ShaderProgram& shader) const {
    glBindTextureUnit(unit, id);
    shader.veci(name, unit);
}

} // namespace gfx

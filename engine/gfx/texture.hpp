#ifndef GFX_TEXTURE_HPP
#define GFX_TEXTURE_HPP

#include "shaders.hpp"

#include "type.hpp"

#include <glad/glad.h>

#include <string>
#include <filesystem>
#include <memory>

namespace gfx {

class Texture {
public:
    class MagFilter : public BaseEnum<GLint>{
    public:
        MagFilter() {}
        MagFilter(GLint flag) : BaseEnum(flag) {}
        operator GLint() {
            return flags;
        }
        constexpr static GLint eNEAREST = GL_NEAREST;
        constexpr static GLint eLINEAR = GL_LINEAR;
    };

    class MinFilter : public MagFilter {
    public:
        MinFilter() {}
        MinFilter(GLint flag) : MagFilter(flag) {}
        operator GLint() {
            return flags;
        }
        constexpr static GLint eNEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST;
        constexpr static GLint eLINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST;
        constexpr static GLint eNEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR;
        constexpr static GLint eLINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR;
    };

    enum class Wrap : GLenum {
        eCLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
        eCLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
        eMIRRORED_REPEAT = GL_MIRRORED_REPEAT,
        eREPEAT = GL_REPEAT,
        eMIRROR_CLAMP_TO_EDGE = GL_MIRROR_CLAMP_TO_EDGE,
    };

    enum class Type : GLenum {
        e2D = GL_TEXTURE_2D,
        e3D = GL_TEXTURE_3D,
        eCUBE_MAP = GL_TEXTURE_CUBE_MAP,
    };

    enum class InternalFormat : GLenum {
        eR8 = GL_R8,
        eR16 = GL_R16,
        eR32F = GL_R32F,
        eRGBA8 = GL_RGBA8,
        eRGBA16F = GL_RGBA16F,
        eRGBA16UI = GL_RGBA16UI,
        eR16UI = GL_R16UI,
        eDEPTH_COMPONENT32 = GL_DEPTH_COMPONENT32,
        eDEPTH_COMPONENT32F = GL_DEPTH_COMPONENT32F,
    };

    enum class Format : GLenum {
        eRGBA = GL_RGBA,
        eRGB = GL_RGB,
        eRED = GL_RED,
        eRG = GL_RG,
        eDEPTH_COMPONENT = GL_DEPTH_COMPONENT,
    };

    enum class DataType : GLenum {
        eFLOAT = GL_FLOAT,
        eUNSIGNED_INT = GL_UNSIGNED_INT,
        eUNSIGNED_BYTE = GL_UNSIGNED_BYTE,
    };

    enum class Swizzle : GLint {
        eR = GL_RED,
        eG = GL_GREEN,
        eB = GL_BLUE,
        eA = GL_ALPHA,
        eONE = GL_ONE,
        eZERO = GL_ZERO,
    };

    enum class CompareFunc : GLint {
        eLEQUAL = GL_LEQUAL,
    };

    enum class CompareMode : GLint {
        eCOMPARE_REF_TO_TEXTURE = GL_COMPARE_REF_TO_TEXTURE,
    };

    // creates the texture object of some type
    // sets all the texture settings
    // gpu allocation is not done in the builder
    struct Builder {
        Wrap wrapS;
        Wrap wrapT;
        Wrap wrapR;
        MinFilter minFilter;
        MagFilter magFilter;

        Swizzle swizzles[4];

        float borderColor[4];

        CompareFunc compareFunc;
        CompareMode compareMode;

        Builder();

        Builder& setMinFilter(MinFilter minFilter);
        Builder& setMagFilter(MagFilter magFilter);
        Builder& setWrapS(Wrap warp);
        Builder& setWrapT(Wrap warp);
        Builder& setWrapR(Wrap warp);
        Builder& setSwizzleR(Swizzle swizzle);
        Builder& setSwizzleG(Swizzle swizzle);
        Builder& setSwizzleB(Swizzle swizzle);
        Builder& setSwizzleA(Swizzle swizzle);
        Builder& setCompareFunc(CompareFunc compareFunc);
        Builder& setCompareMode(CompareMode compareMode);

        // If the texture contains depth components, the first component of GL_TEXTURE_BORDER_COLOR is interpreted as a depth value. The initial value is (0.0,0.0,0.0,0.0).
        Builder& setBorderColor(float r, float g, float b, float a);
        Builder& setBorderColor(float d);

        std::shared_ptr<Texture> build(Type type, const std::string& debugName = "default") const;

        friend class Texture;
    private:
        GLuint getNewID(Type type) const;    
    };

    Texture() = default;
    Texture(GLuint id, Type type, Builder builder);
    ~Texture();

    Texture(const Texture& other) = delete;
    Texture& operator=(const Texture& other) = delete;

    // todo add move semantics
    Texture(Texture&& other);
    Texture&& operator=(Texture&& other);

    struct CreateInfo {
        uint32_t width, height, depth;
        bool genMipMap = true;
        InternalFormat internalFormat;
    };    
    void create(const CreateInfo& info);

    struct UploadInfo {
        Format format;
        DataType dataType;
        int level;
        int xoffset;
        int yoffset;
        int zoffset;
        void *data;
    };
    void upload(const UploadInfo& info);

    void loadImage(std::filesystem::path path);

    void genMipMaps();

    const GLuint getID() const { return id; }

    void bind(const std::string& name, uint32_t unit, ShaderProgram& shader) const;
    void bindImage(const std::string& name, uint32_t unit, uint32_t level, ShaderProgram& shader) const;

    void changeMinFilter(MinFilter minFilter);
    void changeMagFilter(MagFilter magFilter);
    void changeWrapS(Wrap warp);
    void changeWrapT(Wrap warp);
    void changeWrapR(Wrap warp);
    // todo add these vvv
    // void changeSwizzleR(Swizzle swizzle);
    // void changeSwizzleG(Swizzle swizzle);
    // void changeSwizzleB(Swizzle swizzle);
    // void changeSwizzleA(Swizzle swizzle);
    // void changeCompareFunc(CompareFunc compareFunc);
    // void changeCompareMode(CompareMode compareMode);

    // loses pointer to data 
    void resize(CreateInfo& info);

    const CreateInfo& getInfo() const { return info; }

private:
    GLuint id;
    Type type;
    CreateInfo info;
    Builder builder;
};

class CubeMap {
public:
    CubeMap() = default;
    CubeMap(const std::string& imagePath);
    ~CubeMap();
    CubeMap(CubeMap&& texture);
    CubeMap& operator=(CubeMap&& texture);

    CubeMap(CubeMap& texture) = delete;
    CubeMap& operator=(CubeMap& texture) = delete;

    void bind(const std::string& name, uint32_t unit, ShaderProgram& shader) const;

    const GLuint getID() const { return id; }

private:
    GLuint id;
    // GLuint64 handle;
};

} // namespace gfx

#endif
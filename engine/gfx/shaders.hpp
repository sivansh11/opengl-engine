#ifndef GFX_SHADERS_HPP
#define GFX_SHADERS_HPP

#include <glad/glad.h>

#include <vector>
#include <string>
#include <unordered_map>

namespace gfx {

enum UniformType : GLenum {
    eSampler2D = GL_SAMPLER_2D,
    eSampler3D = GL_SAMPLER_3D,
    eSampeler2DShadow = GL_SAMPLER_2D_SHADOW,
    eVec2 = GL_FLOAT_VEC2,
    eVec3 = GL_FLOAT_VEC3,
    eVec4 = GL_FLOAT_VEC4,
    eIvec2 = GL_INT_VEC2,
    eIvec3 = GL_INT_VEC3,
    eIvec4 = GL_INT_VEC4,
    eMat2 = GL_FLOAT_MAT2,
    eMat3 = GL_FLOAT_MAT3,
    eMat4 = GL_FLOAT_MAT4,
    eFloat = GL_FLOAT,
    eInt = GL_INT,
    eBool = GL_BOOL,
    eImage3D = GL_IMAGE_3D,
    eUImage3D = GL_UNSIGNED_INT_IMAGE_3D,
};

class ShaderProgram {
public:
    ShaderProgram();
    ~ShaderProgram();

    ShaderProgram(ShaderProgram&& shaderProgram);
    ShaderProgram& operator=(ShaderProgram&& shaderProgram);

    ShaderProgram(ShaderProgram& shaderProgram) = delete;
    ShaderProgram& operator=(ShaderProgram& shaderProgram) = delete;

    void addShader(const std::string& shaderCodePath);
    void link();

    void bind() const;

    int uniformLocation(const std::string& uniformName);

    void reload();

    void mat4f(const std::string& uniformName, const float *data);
    void mat3f(const std::string& uniformName, const float *data);
    void mat2f(const std::string& uniformName, const float *data);
    void vec4f(const std::string& uniformName, const float *data);
    void vec3f(const std::string& uniformName, const float *data);
    void vec2f(const std::string& uniformName, const float *data);
    void vecf(const std::string& uniformName, const float data);
    void veci(const std::string& uniformName, const int data);

    void dispatchCompute(uint32_t x, uint32_t y, uint32_t z);

    GLuint getID() const { return id; }

    std::unordered_map<std::string, UniformType> uniformTypes;

private:
    GLuint id;
    std::vector<std::string> m_shaderCodePaths;
    std::unordered_map<std::string, int> uniformLocations;
};

} // namespace gfx

#endif
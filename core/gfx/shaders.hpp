#ifndef GFX_SHADERS_HPP
#define GFX_SHADERS_HPP

#include <glad/glad.h>

#include <vector>
#include <string>
#include <unordered_map>

namespace core {

namespace gfx {

class ShaderProgram {
public:
    ShaderProgram();
    ~ShaderProgram();

    ShaderProgram(ShaderProgram&& shaderProgram);
    ShaderProgram& operator=(ShaderProgram&& shaderProgram);

    ShaderProgram(ShaderProgram& shaderProgram) = delete;
    ShaderProgram& operator=(ShaderProgram& shaderProgram) = delete;

    void addShader(const char *shaderCodePath);
    void link();

    void bind();

    int uniformLocation(const char *uniformName);

    void mat4f(const char *uniformName, const float *data);
    void mat3f(const char *uniformName, const float *data);
    void mat2f(const char *uniformName, const float *data);
    void vec4f(const char *uniformName, const float *data);
    void vec3f(const char *uniformName, const float *data);
    void vec2f(const char *uniformName, const float *data);
    void vecf(const char *uniformName, const float data);
    void veci(const char *uniformName, const int data);

    void dispatchCompute(uint32_t x, uint32_t y, uint32_t z);

private:
    GLuint id;
    std::vector<std::string> m_shaderCodePaths;
    std::unordered_map<std::string, int> uniformLocations;
};

} // namespace gfx

} // namespace core

#endif
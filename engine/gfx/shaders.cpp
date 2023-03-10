#include "shaders.hpp"

#include "../core/utils.hpp"

#include <iostream>

namespace gfx {

ShaderProgram::ShaderProgram() {
    id = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(id);
    id = 0;
}

ShaderProgram::ShaderProgram(ShaderProgram&& shaderProgram) {
    id = shaderProgram.id;
    m_shaderCodePaths = std::move(shaderProgram.m_shaderCodePaths);
    uniformLocations = std::move(shaderProgram.uniformLocations);

    shaderProgram.id = {};
    shaderProgram.m_shaderCodePaths = {};
    shaderProgram.uniformLocations = {};
}   

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& shaderProgram) {
    id = shaderProgram.id;
    m_shaderCodePaths = std::move(shaderProgram.m_shaderCodePaths);
    uniformLocations = std::move(shaderProgram.uniformLocations);

    shaderProgram.id = {};
    shaderProgram.m_shaderCodePaths = {};
    shaderProgram.uniformLocations = {};

    return *this;
}   

void ShaderProgram::addShader(const std::string& shaderCodePath) {
    m_shaderCodePaths.emplace_back(shaderCodePath);
}   

void ShaderProgram::link() {
    std::string error = "";
    std::vector<GLuint> shaders;
    shaders.reserve(m_shaderCodePaths.size());

    for (const auto& shaderCodePath : m_shaderCodePaths) {
        // Read shader code from file
        if (error == "") {
            std::string code = core::readFile(shaderCodePath.c_str(), core::FileType::TEXT);
        
            // Determine the shader type from file extension
            GLenum shaderType;
            if (shaderCodePath.find(".vert") != std::string::npos) shaderType = GL_VERTEX_SHADER;
            if (shaderCodePath.find(".frag") != std::string::npos) shaderType = GL_FRAGMENT_SHADER;
            if (shaderCodePath.find(".comp") != std::string::npos) shaderType = GL_COMPUTE_SHADER;
            if (shaderCodePath.find(".geom") != std::string::npos) shaderType = GL_GEOMETRY_SHADER;
            
            GLuint shaderID = glCreateShader(shaderType);
            const char * codePtr = code.c_str();
            glShaderSource(shaderID, 1, &codePtr, NULL);
            glCompileShader(shaderID);

            int  success;
            char infoLog[512];
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
                error = "Failed to compile: " + shaderCodePath + "\n\t" + infoLog;
            }

            if (error == "") {
                glAttachShader(id, shaderID);
                shaders.emplace_back(shaderID);
            }
        }
        
    }
    if (error == "") {
        glLinkProgram(id);

        int success;
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success) {
            GLint logLen;
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLen);
            std::vector<char> log(logLen);
            glGetProgramInfoLog(id, logLen, &logLen, log.data());
            for (auto ch: log) error += ch;
            error += '\n';
        }
    }

    for (const auto& shaderID : shaders) {
        glDeleteShader(shaderID);
    }

    if (error != "") {
        throw std::runtime_error(error);
    }

    int maxLength;
    glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
    GLchar *name = new GLchar[maxLength];

    int uniformCount;
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uniformCount);
    
    for (int i = 0; i < uniformCount; i++) {
        int size;
        GLenum type;
        glGetActiveUniform(id, i, maxLength, nullptr, &size, &type, name);
        std::cout << name << '\n';
        uniformLocation(name);
    }
    delete[] name;
}

void ShaderProgram::reload() {
    glDeleteProgram(id);
    id = glCreateProgram();
    link();
}

void ShaderProgram::bind() const {
    glUseProgram(id);
}

void ShaderProgram::dispatchCompute(uint32_t x, uint32_t y, uint32_t z) {
    glDispatchCompute(x, y, z);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

int ShaderProgram::uniformLocation(const std::string& uniformName) {
    auto foundLocation = uniformLocations.find(uniformName);
    if (foundLocation != uniformLocations.end()) {
        return foundLocation->second;
    }
    int location = glGetUniformLocation(id, uniformName.c_str());
    uniformLocations[uniformName] = location;
    if (location == -1) {
        std::cerr << "Uniform " << uniformName << " requested but does not exist in shaders:\n\t " << m_shaderCodePaths[0].substr(m_shaderCodePaths[0].find_last_of("/") + 1, m_shaderCodePaths[0].find_last_of("/") + 1 + m_shaderCodePaths[0].find_last_of(".")) << "\n";
        // throw std::runtime_error(std::string("Uniform requested but does not exist!") + "\n\t" + uniformName);
    }
    return location;
}

void ShaderProgram::mat4f(const std::string& uniformName, const float *data) {
    glProgramUniformMatrix4fv(id, uniformLocation(uniformName), 1, GL_FALSE, data);
}

void ShaderProgram::mat3f(const std::string& uniformName, const float *data) {
    glProgramUniformMatrix3fv(id, uniformLocation(uniformName), 1, GL_FALSE, data);
}

void ShaderProgram::mat2f(const std::string& uniformName, const float *data) {
    glProgramUniformMatrix2fv(id, uniformLocation(uniformName), 1, GL_FALSE, data);
}

void ShaderProgram::vec4f(const std::string& uniformName, const float *data) {
    glProgramUniform4fv(id, uniformLocation(uniformName), 1, data);
}

void ShaderProgram::vec3f(const std::string& uniformName, const float *data) {
    glProgramUniform3fv(id, uniformLocation(uniformName), 1, data);
}

void ShaderProgram::vec2f(const std::string& uniformName, const float *data) {
    glProgramUniform2fv(id, uniformLocation(uniformName), 1, data);
}

void ShaderProgram::vecf(const std::string& uniformName, const float data) {
    glProgramUniform1f(id, uniformLocation(uniformName), data);
}

void ShaderProgram::veci(const std::string& uniformName, const int data) {
    glProgramUniform1i(id, uniformLocation(uniformName), data);
}

} // namespace gfx

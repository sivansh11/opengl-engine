#include <iostream>

#include "bvh.hpp"
#include "window.hpp"

#include "gfx/shaders.hpp"
#include "gfx/buffer.hpp"
#include "gfx/framebuffer.hpp"
#include "gfx/vertex_attribute.hpp"

#include "utils.hpp"
#include <glad/glad.h>

std::ostream& operator<<(std::ostream& out, glm::vec3& vec) {
    return out << vec.x << ' ' << vec.y << ' ' << vec.z;
}


int main() {
    core::Window window{"BVH test", 640, 420};


    GLuint id = glCreateProgram();

    auto code = core::readFile("../../../apps/bvh_test/shaders/bvh_test.comp", core::TEXT);


    GLuint shaderID = glCreateShader(GL_COMPUTE_SHADER);
    const char *codePtr = code.c_str();
    glShaderSource(shaderID, 1, &codePtr, NULL);
    glCompileShader(shaderID);
    int  success;
    char infoLog[512];
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        std::cout << "Failed to compile: " + std::string("../../../apps/bvh_test/shaders/bvh_test.comp") + "\n\t" + infoLog;
    }

    glAttachShader(id, shaderID);


    glLinkProgram(id);


    // core::gfx::ShaderProgram bvhTest{};
    // bvhTest.addShader("../../../apps/bvh_test/shaders/bvh_test.comp");
    // bvhTest.link();
    // bvhTest.bind();

    // core::bvh::Triangle in_tri {
    //     glm::vec3{0, 1, 2},
    //     glm::vec3{3, 4, 5},
    //     glm::vec3{6, 7, 8},
    // };

    // core::gfx::Buffer in(sizeof(core::bvh::Triangle));
    // in.push(&in_tri);

    // core::gfx::Buffer out(sizeof(core::bvh::Triangle));

    // in.bind(0);
    // out.bind(1);

    // bvhTest.dispatchCompute(1, 1, 1);

    // core::bvh::Triangle out_tri;
    // out.pull(&out_tri);

    // std::cout << in_tri.vert0 << "  " << in_tri.vert1 << "  " << in_tri.vert2 << "\n";
    // std::cout << out_tri.vert0 << "  " << out_tri.vert1 << "  " << out_tri.vert2 << "\n";


    // while (!window.shouldClose()) {
    //     window.pollEvents();



    //     window.updateScreen();
    // }
    
    return 0;
}
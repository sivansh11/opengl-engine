#include <iostream>

#include "bvh.hpp"
#include "window.hpp"

#include "gfx/shaders.hpp"
#include "gfx/buffer.hpp"
#include "gfx/framebuffer.hpp"
#include "gfx/vertex_attribute.hpp"

#include "utils.hpp"
#include <glad/glad.h>

#include <glm/gtc/random.hpp>

int main() {
    core::Window window{"BVH test", 1200, 800};

    core::gfx::ShaderProgram bvhShader{};
    bvhShader.addShader("../../../apps/bvh_test/shaders/bvh.vert");
    bvhShader.addShader("../../../apps/bvh_test/shaders/bvh.frag");
    bvhShader.link();
    bvhShader.bind();

    // bvhShader.vecf("vpWidth", 640);
    // bvhShader.vecf("vpHeight", 420);

    // core::gfx::ShaderProgram bvhShader{};
    // bvhShader.addShader("../../../apps/bvh_test/shaders/bvh_test.comp");
    // bvhShader.link();
    // bvhShader.bind();

    core::gfx::Buffer fbQuad(core::gfx::frameBufferQuadVertices.size() * sizeof(core::gfx::FrameBufferVertex));
    fbQuad.push(core::gfx::frameBufferQuadVertices.data());
    core::gfx::VertexAttribute fbVa;
    fbVa.attributeLocation(0, 2, offsetof(core::gfx::FrameBufferVertex, pos));
    fbVa.attributeLocation(1, 2, offsetof(core::gfx::FrameBufferVertex, uv));
    fbVa.bindVertexBuffer<core::gfx::FrameBufferVertex>(fbQuad);

    // core::gfx::FrameBufferInfo fbInfo{};
    // fbInfo.width = 640;
    // fbInfo.height = 420;
    // fbInfo.attachments.push_back({GL_COLOR_ATTACHMENT0, {GL_RGBA8, GL_UNSIGNED_BYTE}});
    // core::gfx::FrameBuffer fb{fbInfo};

    std::vector<core::bvh::Triangle> tris{
        {{-.5, -.5, 1}, {.5, -.5, 1}, {0, .5, 1}},
        // {{-1.5, -.5, 1}, {-.5, -.5, 1}, {-1, .5, 1}},
    };

    // std::vector<core::bvh::Triangle> tris(64);
    // for (int i = 0; i < 64; i++) {
    //     glm::vec3 r0 = glm::vec3{glm::linearRand(0.f, 1.f)};
    //     glm::vec3 r1 = glm::vec3{glm::linearRand(0.f, 1.f)};
    //     glm::vec3 r2 = glm::vec3{glm::linearRand(0.f, 1.f)};
    //     core::bvh::Triangle tri;
    //     tri.vert0 = r0 * 9.f - glm::vec3{5};
    //     tri.vert1 = tri.vert0 + r1;
    //     tri.vert2 = tri.vert0 + r2;
    //     tris[i] = tri;
    // }

    core::bvh::Bvh bvh{};
    bvh.builder(tris);

    auto gpuBufferData = bvh.getGpuBufferData();

    core::gfx::Buffer triangles{gpuBufferData.triangleCount * (uint32_t)sizeof(core::bvh::Triangle), core::gfx::DYNAMIC_BUFFER};
    core::gfx::Buffer triangleIds{gpuBufferData.triangleCount * (uint32_t)sizeof(uint32_t), core::gfx::DYNAMIC_BUFFER};
    core::gfx::Buffer bvhNode{gpuBufferData.triangleCount * 2 * (uint32_t)sizeof(core::bvh::BvhNode), core::gfx::DYNAMIC_BUFFER};
 
    triangles.push(gpuBufferData.triangleBuffer);
    triangleIds.push(gpuBufferData.idBuffer);
    bvhNode.push(gpuBufferData.bvhNodeBuffer);

    std::cout << gpuBufferData.triangleCount * (uint32_t)sizeof(core::bvh::Triangle) << '\n';
    std::cout << gpuBufferData.triangleCount * (uint32_t)sizeof(uint32_t) << '\n';
    std::cout << gpuBufferData.triangleCount * 2 * (uint32_t)sizeof(core::bvh::BvhNode) << '\n';

    triangles.bind(0);
    triangleIds.bind(1);
    bvhNode.bind(2);

    // core::gfx::Buffer val(1 * sizeof(float));
    // val.bind(3);

    // bvhShader.dispatchCompute(1, 1, 1);

    // std::cout << rets[0] << ' ' << rets[1] << ' ' << rets[2] << '\n';

    while (!window.shouldClose()) {
        window.pollEvents();

        glClearColor(1, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // fb.bind();
        bvhShader.bind();
        // bvhShader.veci("vpWidth", 640);
        // bvhShader.veci("vpHeight", 420);
        int w, h;
        glfwGetWindowSize(window.getWindow(), &w, &h);
        bvhShader.vecf("aspectRatio", float(w) / float(h));
        fbVa.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        window.updateScreen();
    }


    // float rets[1];
    // val.pull(&rets);

    // std::cout << rets[0] << '\n';

    return 0;
}
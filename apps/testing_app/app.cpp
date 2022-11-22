#include "app.hpp"

#include "gfx/vertex_attribute.hpp"
#include "gfx/buffer.hpp"
#include "gfx/texture.hpp"
#include "gfx/shaders.hpp"
#include "gfx/framebuffer.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "material.hpp"
#include "imgui_utils.hpp"
#include "camera.hpp"
#include "controller.hpp"
#include "transform.hpp"
#include "light.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <unordered_set>

namespace app {

App::App() : window("App", 1600, 900) {
    core::initImgui(window.getWindow());
    glEnable(GL_DEPTH_TEST);
}

App::~App() {
    core::terminateImgui();
}

void App::run() {
    // core::ShaderProgram shader{};
    // shader.addShader("../../../apps/testing_app/shaders/blinn-phong.vert");
    // shader.addShader("../../../apps/testing_app/shaders/blinn-phong.frag");
    // shader.link();

    core::Model model{"../../../models/2.0/Sponza/glTF/Sponza.gltf"};
    core::Transform modelTransform{};
    modelTransform.scale = {.01, .01, .01};

    core::Transform cameraTransform{};
    core::Camera    camera;
    camera.setPerspectiveProjection(60.0f, window.getAspect(), 0.01, 100);
    core::Controller controller;

    std::vector<core::PointLight> pointLight(1);
    for (int i = 0; i < pointLight.size(); i++) {
        pointLight[i].pos = glm::vec3(1, 1, 1);
        pointLight[i].ambient = {0.3, 0.3, 0.3};
        pointLight[i].diffuse = {.7, .7, .7};
        pointLight[i].term = {0.01, 0.01, 0.01}
    }
    // pointLight.pos = {1, 1, 1};

    core::Buffer pointLights(sizeof(core::PointLight) * pointLight.size(), core::BufferType::DYNAMIC_BUFFER);
    pointLights.push(pointLight.data());

    core::FrameBufferInfo gBufferInfo;
    gBufferInfo.width = 1600, gBufferInfo.height = 900;
    gBufferInfo.attachments.push_back({GL_COLOR_ATTACHMENT0, {GL_RGBA16F, GL_FLOAT}});
    gBufferInfo.attachments.push_back({GL_COLOR_ATTACHMENT1, {GL_RGBA16F, GL_FLOAT}});
    gBufferInfo.attachments.push_back({GL_COLOR_ATTACHMENT2, {GL_RGBA8, GL_UNSIGNED_BYTE}});
    gBufferInfo.attachments.push_back({GL_DEPTH_ATTACHMENT, {GL_DONT_CARE, GL_DONT_CARE}});
    core::FrameBuffer gBuffer(gBufferInfo);

    core::ShaderProgram geometryPass{};
    geometryPass.addShader("../../../apps/testing_app/shaders/gbuffer.vert");
    geometryPass.addShader("../../../apps/testing_app/shaders/gbuffer.frag");
    geometryPass.link();

    core::Buffer fbQuad(sizeof(core::FrameBufferVertex) * core::frameBufferQuadVertices.size());
    fbQuad.push((void *)core::frameBufferQuadVertices.data());
    core::VertexAttribute fbQuadVA;
    fbQuadVA.attributeLocation(0, 2, offsetof(core::FrameBufferVertex, pos));
    fbQuadVA.attributeLocation(1, 2, offsetof(core::FrameBufferVertex, uv));
    fbQuadVA.bindVertexBuffer<core::FrameBufferVertex>(fbQuad);

    core::ShaderProgram lightPass{};
    lightPass.addShader("../../../apps/testing_app/shaders/light_pass.vert");
    lightPass.addShader("../../../apps/testing_app/shaders/light_pass.frag");
    lightPass.link();

    lightPass.bind();
    lightPass.veci("gBuffer.posBuffer", 0);
    lightPass.veci("gBuffer.norBuffer", 1);
    lightPass.veci("gBuffer.albedoSpecBuffer", 2);
    // core::ShaderProgram screen;  
    // screen.addShader("../../../apps/testing_app/shaders/gBuffer-screen.vert");
    // screen.addShader("../../../apps/testing_app/shaders/gBuffer-screen.frag");
    // screen.link();

    auto map = pointLights.map();

    double previous = glfwGetTime();

    while (!window.shouldClose()) {
        window.pollEvents();

        double currentTime = glfwGetTime();
        double deltaTime = currentTime - previous;

        previous = currentTime;

        if (glfwGetKey(window.getWindow(), GLFW_KEY_L) != GLFW_PRESS) {
            controller.move(window.getWindow(), static_cast<float>(deltaTime), cameraTransform.translation, cameraTransform.rotation);
            camera.setViewYXZ(cameraTransform.translation, cameraTransform.rotation);
        } else {
            glm::vec3 temp;
            controller.move(window.getWindow(), static_cast<float>(deltaTime), pointLight[0].pos, temp);
            std::memcpy(map, &pointLight[0], sizeof(core::PointLight) * pointLight.size());
        }

        glm::mat4 pv = camera.getProjection() * camera.getView();

        gBuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        geometryPass.bind();
        geometryPass.mat4f("mvp.model", glm::value_ptr(modelTransform.mat4()));
        geometryPass.mat4f("mvp.view", glm::value_ptr(camera.getView()));
        geometryPass.mat4f("mvp.projection", glm::value_ptr(camera.getProjection()));
        model.draw(geometryPass);
        gBuffer.unbind();

        glClearColor(1, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightPass.bind();
        lightPass.vec3f("viewPos", glm::value_ptr(cameraTransform.translation));
        glBindTextureUnit(0, gBuffer.getTextureID(GL_COLOR_ATTACHMENT0));
        glBindTextureUnit(1, gBuffer.getTextureID(GL_COLOR_ATTACHMENT1));
        glBindTextureUnit(2, gBuffer.getTextureID(GL_COLOR_ATTACHMENT2));
        pointLights.bind(0);    
        fbQuadVA.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // screen.bind();
        // screen.veci("albedoSpec", 2);
        // glBindTextureUnit(2, gBuffer.getTextureID(core::FrameBufferAttachmentType::COLOR_ATTACHMENT_2));
        // fbQuadVA.bind();
        // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


        // shader.bind();
        // pointLights.bind(0);
        // shader.mat4f("mvp.model", glm::value_ptr(modelTransform.mat4()));
        // shader.mat4f("mvp.view", glm::value_ptr(camera.getView()));
        // shader.mat4f("mvp.projection", glm::value_ptr(camera.getProjection()));
        // shader.vec3f("viewPos", glm::value_ptr(cameraTransform.translation));

        // model.draw(shader);

        core::startFrameImgui();

        #ifndef NDEBUG
        ImGui::Begin("Debug Build");
        ImGui::Text("%f fps %f ms / per frame", ImGui::GetIO().Framerate, 1000 * ImGui::GetIO().DeltaTime);
        ImGui::End();
        #else
        ImGui::Begin("Release Build");
        ImGui::Text("%f fps %f ms / per frame", ImGui::GetIO().Framerate, 1000 * ImGui::GetIO().DeltaTime);
        ImGui::End();
        #endif

        ImGui::Begin("Textures");
        ImGui::Image((ImTextureID)gBuffer.getTextureID(GL_COLOR_ATTACHMENT0), {100, 100}, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::NewLine();
        ImGui::Image((ImTextureID)gBuffer.getTextureID(GL_COLOR_ATTACHMENT1), {100, 100}, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::NewLine();
        ImGui::Image((ImTextureID)gBuffer.getTextureID(GL_COLOR_ATTACHMENT2), {100, 100}, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        core::endFrameImgui(window.getWindow());

        window.updateScreen();
    }

    pointLights.unmap(map);
}

} // namespace app

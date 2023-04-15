#include "app.hpp"

#include "renderer/renderpass/test.hpp"
#include "renderer/pipeline/basic_pipeline.hpp"
#include "renderer/pipeline/deferred_pipeline.hpp"
#include "renderer/renderpass/basic_renderpass.hpp"
#include "renderer/renderpass/geomtery_pass.hpp"
#include "renderer/renderpass/composite_renderpass.hpp"
#include "renderer/model.hpp"

#include "core/event.hpp"

#include "core/components.hpp"

#include "core/imgui_utils.hpp"

#include "editor_camera.hpp"

#include "gfx/framebuffer.hpp"

#include <entt/entt.hpp>

#include <iostream>

#define ASSETS_PATH std::string("../../../assets")

App::App() : window("Editor", 800, 600) {
    core::initImgui(window);
    // core::setupImGuiStyle();
    // core::myDefaultImGuiStyle();
}

App::~App() {
    core::terminateImgui();
}

void App::run() {

    entt::registry registry;
    event::Dispatcher dispatcher;

    EditorCamera camera;
    uint32_t width = 100, height = 100;

    {
        auto ent = registry.create();
        registry.emplace<renderer::Model>(ent).loadModelFromPath("../../../assets/models/2.0/Sponza/glTF/Sponza.gltf");
    }

    {
        auto ent = registry.create();
        auto& pl = registry.emplace<core::PointLightComponent>(ent);
        pl.position = {6, 1, 0};
        pl.color = {1, 1, 1};
        pl.term = {.3, .3, .1};
    }

    std::shared_ptr<renderer::RenderPass> geometryPass = std::make_shared<renderer::GeometryPass>();
    renderer::DeferredPipeline deferredPipeline{dispatcher};
    deferredPipeline.addRenderPass(geometryPass);

    std::shared_ptr<renderer::RenderPass> compositePass = std::make_shared<renderer::CompositePass>();
    renderer::BasicPipeline basicPipeline{dispatcher};
    basicPipeline.addRenderPass(compositePass);

    double lastTime = glfwGetTime();

    while (!window.shouldClose()) {
        window.pollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double currentTime = glfwGetTime(); 
        float dt = currentTime - lastTime;
        lastTime = currentTime;
        
        renderer::RenderContext renderContext;
        renderContext["width"] = width;
        renderContext["height"] = height;
        renderContext["view"] = camera.getView();
        renderContext["projection"] = camera.getProjection();
        renderContext["viewPos"] = camera.getPos();

        deferredPipeline.render(registry, renderContext);
        basicPipeline.render(registry, renderContext);

        core::startFrameImgui();

        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None & ~ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiWindowFlags windowFlags = 
                                       ImGuiWindowFlags_NoDocking | 
                                       ImGuiWindowFlags_NoTitleBar |
                                       ImGuiWindowFlags_NoCollapse |
                                       ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus |
                                       ImGuiWindowFlags_NoNavFocus |
                                       ImGuiWindowFlags_NoBackground |
                                       ImGuiWindowFlags_NoDecoration;

        bool dockSpace = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        auto mainViewPort = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(mainViewPort->WorkPos);
        ImGui::SetNextWindowSize(mainViewPort->WorkSize);
        ImGui::SetNextWindowViewport(mainViewPort->ID);

        ImGui::Begin("DockSpace", &dockSpace, windowFlags);
        ImGuiID dockspaceID = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockspaceID, ImGui::GetContentRegionAvail(), dockspaceFlags);
        // if (ImGui::BeginMainMenuBar()) {
        //     if (ImGui::BeginMenu("Panels")) {
        //         for (auto& panel : panels) {
        //             if (ImGui::MenuItem(panel->getName().c_str(), NULL, &panel->getShow())) {}
        //         }
        //         ImGui::EndMenu();
        //     }
        //     ImGui::EndMainMenuBar();
        // }
        ImGui::End();
        ImGui::PopStyleVar(2);

        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
        ImGui::SetNextWindowClass(&window_class);
        ImGuiWindowFlags viewPortFlags = ImGuiWindowFlags_NoTitleBar |
                                         ImGuiWindowFlags_NoDecoration;
        ImGui::Begin("viewport", nullptr, viewPortFlags);
        auto vp = ImGui::GetWindowSize();
        width = vp.x;
        height = vp.y;
        camera.onUpdate(dt);
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(std::any_cast<GLuint>(renderContext["finalImage"]))), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
    
        ImGui::End();
        ImGui::PopStyleVar(2);

        core::endFrameImgui(window);
        
        window.updateScreen();
    }

}
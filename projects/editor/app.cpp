#include "app.hpp"

#include "core/imgui_utils.hpp"

#include "renderer/renderpass/test.hpp"
#include "renderer/pipeline/forward_pipeline.hpp"
#include "renderer/pipeline/deferred_pipeline.hpp"
#include "renderer/renderpass/geomtery_pass.hpp"
#include "renderer/renderpass/composite_pass.hpp"
#include "renderer/renderpass/shadowmap_pass.hpp"
#include "renderer/pipeline/shadow_pipeline.hpp"
#include "renderer/pipeline/ssao_pipeline.hpp"
#include "renderer/renderpass/ssao_pass.hpp"
#include "renderer/renderpass/voxelize_pass.hpp"
#include "renderer/renderpass/vxgi_pass.hpp"
#include "renderer/pipeline/voxel_pipeline.hpp"
#include "renderer/pipeline/visualization_pipeline.hpp"
#include "renderer/renderpass/visualize_voxels_pass.hpp"
#include "renderer/renderpass/vxgi_composite_pass.hpp"
#include "renderer/model.hpp"

#include "testing_panel.hpp"

#include "core/event.hpp"

#include "core/components.hpp"

#include "editor_camera.hpp"

#include "gfx/framebuffer.hpp"
#include "gfx/timer.hpp"

#include <entt/entt.hpp>

#include <iostream>
#include <chrono>
#include <thread>

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

    std::shared_ptr<renderer::RenderPass> geometryPass = std::make_shared<renderer::GeometryPass>(dispatcher);
    renderer::DeferredPipeline deferredPipeline{dispatcher};
    deferredPipeline.addRenderPass(geometryPass);

    std::shared_ptr<renderer::RenderPass> compositePass = std::make_shared<renderer::CompositePass>(dispatcher);
    renderer::ForwardPipeline forwardPipeline{dispatcher};
    forwardPipeline.addRenderPass(compositePass);

    std::shared_ptr<renderer::RenderPass> shadowMapPass = std::make_shared<renderer::ShadowMapPass>(dispatcher);
    renderer::ShadowPipeline shadowPipeline{dispatcher};
    shadowPipeline.addRenderPass(shadowMapPass);

    std::shared_ptr<renderer::RenderPass> ssaoPass = std::make_shared<renderer::SSAOPass>(dispatcher);
    renderer::SSAOPipeline ssaoPipeline{dispatcher};
    ssaoPipeline.addRenderPass(ssaoPass);

    std::shared_ptr<renderer::RenderPass> voxelizePass = std::make_shared<renderer::VoxelizationPass>(dispatcher);
    std::shared_ptr<renderer::RenderPass> vxgiPass = std::make_shared<renderer::VXGICompositePass>(dispatcher);
    renderer::VoxelPipeline voxelPipeline{dispatcher};
    voxelPipeline.addRenderPass(voxelizePass);
    voxelPipeline.addRenderPass(vxgiPass);

    std::shared_ptr<renderer::RenderPass> visualizePass = std::make_shared<renderer::VisualizePass>(dispatcher);
    renderer::VisualizationPipeline visualizationPipeline{dispatcher};
    visualizationPipeline.addRenderPass(visualizePass);

    std::vector<core::BasePanel *> pipelines;
    pipelines.push_back(&deferredPipeline);
    // pipelines.push_back(&forwardPipeline);
    pipelines.push_back(&shadowPipeline);
    // pipelines.push_back(&ssaoPipeline);
    pipelines.push_back(&voxelPipeline);
    pipelines.push_back(&visualizationPipeline);
    
    // for (auto pipeline : pipelines) {
    //     pipeline->show = false;
    // }

    shadowPipeline.show = true;
    voxelPipeline.show = true;

    {
        auto ent = registry.create();
        registry.emplace<renderer::Model>(ent).loadModelFromPath("../../../assets/models/2.0/Sponza/glTF/Sponza.gltf");
        // registry.emplace<renderer::Model>(ent).loadModelFromPath("../../../assets/models/2.0/ABeautifulGame/glTF/ABeautifulGame.gltf");
        auto& t = registry.emplace<core::TransformComponent>(ent);
        t.scale = {1, 1, 1};
    }

    // {
    //     auto ent = registry.create();
    //     auto& pl = registry.emplace<core::PointLightComponent>(ent);
    //     pl.position = {6, 1, 0};
    //     pl.color = {2, 2, 2};
    //     pl.term = {.3, .3, .1};
    // }

    {
        auto ent = registry.create();
        auto& dl = registry.emplace<core::DirectionalLightComponent>(ent);
    }

    ViewPanel viewPanel;

    std::vector<core::BasePanel *> panels;
    panels.push_back(&viewPanel);

    double lastTime = glfwGetTime();

    float targetFPS = 60.f;

    gfx::AsyncTimerQuery timer{5};

    while (!window.shouldClose()) {
        window.pollEvents();

        double currentTime = glfwGetTime(); 
        float dt = currentTime - lastTime;
        if (currentTime - lastTime < 1.f / targetFPS) {
            continue;
        } 
        lastTime = currentTime;  

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            dispatcher.post<core::ReloadShaderEvent>();
        }
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
            targetFPS = 500;
        } else {
            targetFPS = 60;
        }
        

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        renderer::RenderContext renderContext;
        renderContext.at("width") = width;
        renderContext.at("height") = height;
        renderContext.at("view") = camera.getView();
        renderContext.at("invView") = glm::inverse(camera.getView());
        renderContext.at("projection") = camera.getProjection();
        renderContext.at("invProjection") = glm::inverse(camera.getProjection());
        renderContext.at("viewPos") = camera.getPos();
        renderContext.at("viewDir") = camera.getDir();
        renderContext.at("showing") = viewPanel.selectedImage;

        timer.begin();

        shadowPipeline.render(registry, renderContext);
        deferredPipeline.render(registry, renderContext);
        voxelPipeline.render(registry, renderContext);
        // ssaoPipeline.render(registry, renderContext);
        visualizationPipeline.render(registry, renderContext);
        // forwardPipeline.render(registry, renderContext);

        timer.end();

        if (auto time = timer.popTimeStamp()) {
            std::cout << time.value() / 1000000.0 << "ms" << '\n';
        }

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
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Pipelines")) {
                for (auto& panel : pipelines) {
                    if (ImGui::MenuItem(panel->m_name.c_str(), NULL, &panel->show)) {}
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Panels")) {
                for (auto& panel : panels) {
                    if (ImGui::MenuItem(panel->m_name.c_str(), NULL, &panel->show)) {}
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
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
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(renderContext.at(viewPanel.selectedImage).as<std::shared_ptr<gfx::Texture>>()->getID())), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
    
        ImGui::End();
        ImGui::PopStyleVar(2);

        for (auto panel : pipelines) {
            panel->uiPanel();
        }

        for (auto panel : panels) {
            panel->uiPanel();
        }

        ImGui::Begin("Debug");
        ImGui::Text("%f", ImGui::GetIO().Framerate);
        ImGui::End();

        core::endFrameImgui(window);
        
        window.updateScreen();
    }

    std::cout << glGetString(GL_RENDERER) << '\n';

}
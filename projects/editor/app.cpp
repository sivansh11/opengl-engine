#include "app.hpp"

#include "core/imgui_utils.hpp"

#include "renderer/renderpass/test.hpp"
#include "renderer/pipeline/forward_pipeline.hpp"
#include "renderer/pipeline/deferred_pipeline.hpp"
#include "renderer/renderpass/geomtery_pass.hpp"
#include "renderer/renderpass/composite_renderpass.hpp"
#include "renderer/renderpass/depth_renderpass.hpp"
#include "renderer/pipeline/depth_pipeline.hpp"
#include "renderer/pipeline/ssao_pipeline.hpp"
#include "renderer/renderpass/ssao_pass.hpp"
#include "renderer/model.hpp"

#include "core/event.hpp"

#include "core/components.hpp"

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

    // {
    //     auto ent = registry.create();
    //     auto& pl = registry.emplace<core::PointLightComponent>(ent);
    //     pl.position = {6, 1, 0};
    //     pl.color = {1, 1, 1};
    //     pl.term = {.3, .3, .1};
    // }

    {
        auto ent = registry.create();
        auto& dl = registry.emplace<core::DirectionalLightComponent>(ent);
    }

    std::shared_ptr<renderer::RenderPass> geometryPass = std::make_shared<renderer::GeometryPass>();
    renderer::DeferredPipeline deferredPipeline{dispatcher};
    deferredPipeline.addRenderPass(geometryPass);

    std::shared_ptr<renderer::RenderPass> compositePass = std::make_shared<renderer::CompositePass>();
    renderer::ForwardPipeline forwardPipeline{dispatcher};
    forwardPipeline.addRenderPass(compositePass);

    std::shared_ptr<renderer::RenderPass> depthPass = std::make_shared<renderer::DepthPass>();
    renderer::DepthPipeline depthPipeline{dispatcher};
    depthPipeline.addRenderPass(depthPass);

    std::shared_ptr<renderer::RenderPass> ssaoPass = std::make_shared<renderer::SSAOPass>();
    renderer::SSAOPipeline ssaoPipeline{dispatcher};
    ssaoPipeline.addRenderPass(ssaoPass);

    std::vector<core::BasePanel *> pipelines;
    pipelines.push_back(&deferredPipeline);
    pipelines.push_back(&forwardPipeline);
    pipelines.push_back(&depthPipeline);
    pipelines.push_back(&ssaoPipeline);
    
    for (auto pipeline : pipelines) {
        pipeline->show = false;
    }

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

        depthPipeline.render(registry, renderContext);
        deferredPipeline.render(registry, renderContext);
        ssaoPipeline.render(registry, renderContext);
        forwardPipeline.render(registry, renderContext);

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
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["finalImage"])->getID())), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
    
        ImGui::End();
        ImGui::PopStyleVar(2);

        for (auto panel : pipelines) {
            panel->uiPanel();
        }

        ImGui::Begin("Debug");
        ImGui::Text("%f", ImGui::GetIO().Framerate);
        ImGui::End();

        core::endFrameImgui(window);
        
        window.updateScreen();
    }

}
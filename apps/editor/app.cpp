#include "app.hpp"

#include "gfx/shaders.hpp"
#include "gfx/framebuffer.hpp"
#include "gfx/buffer.hpp"
#include "gfx/vertex_attribute.hpp"
#include "renderer/model.hpp"

#include "core/entity.hpp"
#include "core/serializer.hpp"
#include "core/components.hpp"
#include "core/execution_queue.hpp"

#include "scene_renderer.hpp"
#include "scene_renderer_gi.hpp"

#include "core/imgui_utils.hpp"
#include "renderer/mesh.hpp"

#include "editor_camera.hpp"

#include "panels/content_browser_panel.hpp"
#include "panels/editor_color_picker_panel.hpp"
#include "panels/entity_view_panel.hpp"
#include "panels/frame_info_panel.hpp"
#include "panels/scene_hierarchy_panel.hpp"
#include "panels/settings_panel.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#define ASSETS_PATH std::string("../../../assets")

App::App() : window("Editor", 1600, 800) {
    core::initImgui(window);
    core::setupImGuiStyle();
    core::myDefaultImGuiStyle();
}

App::~App() {
    core::terminateImgui();
}

void App::run() {
    assert(ecs::getComponentID<core::TagComponent>() != ecs::getComponentID<core::PointLightComponent>());
    assert(ecs::getComponentID<core::TagComponent>() != ecs::getComponentID<core::DirectionalLightComponent>());
    
    SceneRendererGI renderer(window);
    
    std::shared_ptr<core::Scene> scene = std::make_shared<core::Scene>();

    EditorCamera camera;

    ImVec2 size{100, 100};

    std::vector<core::Panel *> panels;

    SceneHierarchyPanel sceneHierarchyPanel(window.getDispatcher()); 
    sceneHierarchyPanel.setSceneContext(&*scene);
    EntityViewPanel entityViewPanel{window.getDispatcher()};
    ContentBrowserPanel contentBrowserPanel{ASSETS_PATH};
    EditorColorPickerPanel editorColorPickerPanel{};
    FrameInfoPanel frameInfoPanel{};
    SettingsPanel settingsPanel{window};

    panels.push_back(&sceneHierarchyPanel);
    panels.push_back(&entityViewPanel);
    panels.push_back(&contentBrowserPanel);
    panels.push_back(&editorColorPickerPanel);
    panels.push_back(&frameInfoPanel);
    panels.push_back(&settingsPanel);
    panels.push_back(&renderer);

    double lastTime = glfwGetTime();

    while (!window.shouldClose()) {
        window.pollEvents();

        double currentTime = glfwGetTime(); 
        if (!(currentTime - lastTime >= 1.0 / settingsPanel.getFPS())) continue;
        float dt = currentTime - lastTime;
        lastTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT);
        
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
                                       ImGuiWindowFlags_NoDecoration
                                       ;
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
            if (ImGui::BeginMenu("Panels")) {
                for (auto& panel : panels) {
                    if (ImGui::MenuItem(panel->getName().c_str(), NULL, &panel->getShow())) {}
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
        ImVec2 vpSize = ImGui::GetContentRegionAvail();
        if (vpSize.x != size.x || vpSize.y != size.y) {
            size = vpSize;
            window.getDispatcher().post<ViewPortResizeEvent>(static_cast<uint32_t>(vpSize.x), static_cast<uint32_t>(vpSize.y));
        }
        camera.onUpdate(dt);
        renderer.render(scene, camera, {vpSize.x, vpSize.y});
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void*>(renderer.getRenderedImage())), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
        if (ImGui::BeginDragDropTarget()) {
            const ImGuiPayload *payLoad = ImGui::AcceptDragDropPayload("FILE_PATH");
            if (payLoad) {
                std::string filePath = std::string(reinterpret_cast<const char *>(payLoad->Data), reinterpret_cast<const char *>(payLoad->Data) + payLoad->DataSize);
                if (filePath.find(".hs") != std::string::npos) {
                    scene = nullptr;
                    scene = core::Serializer::loadScene(filePath);
                    sceneHierarchyPanel.setSceneContext(&*scene);
                    entityViewPanel.setSceneContext(&*scene);
                    entityViewPanel.setSelectecEntity(ecs::null);
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::End();
        ImGui::PopStyleVar(2);

        for (auto panel : panels) {
            panel->onImGuiRender();
        }

        core::endFrameImgui(window);

        window.updateScreen();
    }

    core::Serializer::saveScene(scene, ASSETS_PATH + "/scenes/test.hs");
}
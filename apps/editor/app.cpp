#include "app.hpp"

#include "gfx/shaders.hpp"
#include "gfx/framebuffer.hpp"
#include "gfx/buffer.hpp"
#include "gfx/vertex_attribute.hpp"
#include "renderer/model.hpp"

#include "core/entity.hpp"
#include "core/serializer.hpp"

#include "core/components.hpp"

#include "scene_renderer.hpp"
#include "adv_scene_renderer.hpp"

#include "core/imgui_utils.hpp"
#include "renderer/mesh.hpp"

#include "editor_camera.hpp"

#include "panels/content_browser_panel.hpp"
#include "panels/editor_color_picker_panel.hpp"
#include "panels/entity_view_panel.hpp"
#include "panels/frame_info_panel.hpp"
#include "panels/scene_hierarchy_panel.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#define ASSETS_PATH std::string("../../../assets")

App::App() : window("Test", 1600, 800) {
    core::initImgui(window);
    core::setupImGuiStyle();
}

App::~App() {
    core::terminateImgui();
}

void App::run() {
    AdvSceneRenderer renderer(window);
    
    std::shared_ptr<core::Scene> scene = core::Serializer::loadScene(ASSETS_PATH + "/scenes/test.hs");

    EditorCamera camera;

    ImVec2 size{100, 100};

    std::vector<Panel *> panels;

    SceneHierarchyPanel sceneHierarchyPanel(window.getDispatcher()); 
    sceneHierarchyPanel.setSceneContext(&*scene);
    EntityViewPanel entityViewPanel{window.getDispatcher()};
    ContentBrowserPanel contentBrowserPanel{ASSETS_PATH};
    EditorColorPickerPanel editorColorPickerPanel{};
    FrameInfoPanel frameInfoPanel{};

    panels.push_back(&sceneHierarchyPanel);
    panels.push_back(&entityViewPanel);
    panels.push_back(&contentBrowserPanel);
    panels.push_back(&editorColorPickerPanel);
    panels.push_back(&frameInfoPanel);

    struct Settings {
        float fps = 144;
    } settings;

    double lastTime = glfwGetTime();

    while (!window.shouldClose()) {
        window.pollEvents();

        double currentTime = glfwGetTime(); 

        if (!(currentTime - lastTime >= 1.0 / settings.fps)) continue;
        
        float dt = currentTime - lastTime;

        lastTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT);
        
        renderer.render(scene, camera);

        core::startFrameImgui();

        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None & ~ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar |
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
                // if (ImGui::MenuItem("Scene Hierarchy Panel", NULL, &sceneHierarchyPanel.getShow())) {}
                // if (ImGui::MenuItem("Registered Scripts Panel", NULL, &registeredScriptsPanel.getShow())) {}
                // if (ImGui::MenuItem("Entity View Panel", NULL, &entityViewPanel.getShow())) {}
                // if (ImGui::MenuItem("Content Browser Panel", NULL, &contentBrowserPanel.getShow())) {}
                // if (ImGui::MenuItem("Editor Color Picker Panel", NULL, &editorColorPickerPanel.getShow())) {}
                // if (ImGui::MenuItem("Frame Info Panel", NULL, &frameInfoPanel.getShow())) {}
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
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void*>(renderer.getRender())), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        ImGui::PopStyleVar(2);

        for (auto panel : panels) {
            panel->renderPanel();
        }

        entityViewPanel.setSceneContext(&*scene);

        ImGui::Begin("Debug");
        renderer.imguiRender();
        ImGui::End();

        core::endFrameImgui(window);

        window.updateScreen();
    }

    core::Serializer::saveScene(scene, ASSETS_PATH + "/scenes/test.hs");
}
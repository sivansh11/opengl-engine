#include "app.hpp"

#include "imgui_utils.hpp"
#include "pyscript/script_engine.hpp"
#include "entity.hpp"
#include "event_types.hpp"
#include "scene_renderer.hpp"

#include "panels/scene_hierarchy_panel.hpp"
#include "panels/entity_view_panel.hpp"
#include "panels/content_browser_panel.hpp"
#include "panels/registered_script_panel.hpp"
#include "panels/editor_color_picker_panel.hpp"
#include "panels/frame_info_panel.hpp"

#include "gfx/shaders.hpp"
#include "gfx/buffer.hpp"

App::App() : window("Editor", 1600, 800) {
    core::initImgui(window.getWindow());
    core::setupImGuiStyle();
    core::myDefaultImGuiStyle();
    float size_pixels = 12;
    ImFont *font = ImGui::GetIO().Fonts->AddFontFromFileTTF("../../../apps/editor/fonts/CooperHewitt-Medium.otf", size_pixels);
    ImGui::GetIO().FontDefault = font;
    core::pyscript::ScriptEngine::init();
    // core::pyscript::ScriptEngine::exec_script("../../../apps/editor/scripts/test.py");
}

App::~App() {
    core::pyscript::ScriptEngine::free();
    core::terminateImgui();
}

void App::run() {

    core::Scene scene;
    event::Dispatcher dispatcher;

    auto model = scene.createEntity();
    model.assign<core::TagComponent>("Model");
    model.assign<core::Model>("../../../models/2.0/Sponza/glTF/Sponza.gltf");
    auto& m = model.assign<core::TransformComponent>();
    m.scale = {0.01, 0.01, 0.01};
    m.translation = {0, 0, 0.3};
    // model.assign<core::Model>("../../../models/2.0/FlightHelmet/glTF/FlightHelmet.gltf");
    // model.assign<core::TransformComponent>();
    {
        auto light = scene.createEntity();
        light.assign<core::PointLightComponent>();
        light.assign<core::AmbienceLightComponent>().ambient = {0.63, 0.63, 0.63};
        light.assign<core::TransformComponent>().translation = {0, 2, 0};
        light.assign<core::TagComponent>("Point Light");
    }
    // {
    //     auto light = scene.createEntity();
    //     light.assign<core::PointLightComponent>();
    //     light.assign<core::TransformComponent>();
    //     light.assign<core::AmbienceLightComponent>();
    //     light.assign<core::TagComponent>("Point Light");
    // }

    SceneRenderer renderer{dispatcher};

    core::EditorCamera editorCamera;

    ImVec2 size{100, 100};

    std::vector<core::Panel *> panels;

    core::SceneHierarchyPanel sceneHierarchyPanel(dispatcher); 
    sceneHierarchyPanel.setSceneContext(&scene.getScene());
    core::RegisteredScriptsPanel registeredScriptsPanel{};
    core::EntityViewPanel entityViewPanel{dispatcher};
    core::ContentBrowserPanel contentBrowserPanel{std::filesystem::current_path().parent_path().parent_path().parent_path()};
    core::EditorColorPickerPanel editorColorPickerPanel{};
    core::FrameInfoPanel frameInfoPanel{};

    panels.push_back(&sceneHierarchyPanel);
    panels.push_back(&registeredScriptsPanel);
    panels.push_back(&entityViewPanel);
    panels.push_back(&contentBrowserPanel);
    panels.push_back(&editorColorPickerPanel);
    panels.push_back(&frameInfoPanel);

    struct Settings {
        float fps = 60;
    } settings;

    double lastTime = glfwGetTime();
    while (!window.shouldClose()) {
        double currentTime = glfwGetTime(); 

        if (!(currentTime - lastTime >= 1.0 / settings.fps)) continue;
        
        lastTime = currentTime;
        
        window.pollEvents();

        float ts = ImGui::GetIO().DeltaTime;

        for (auto [ent, scripts] : scene.getScene().group<core::ScriptComponent>()) {
            for (auto& kv : scripts.scripts) {
                kv.second.attr("on_update")(ts);
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        renderer.render(scene.getScene(), editorCamera);

        if (ImGui::IsKeyPressed(ImGuiKey_L)) {
            auto newLight = scene.createEntity();
            newLight.assign<core::TagComponent>("New Light");
            newLight.assign<core::PointLightComponent>();
            newLight.assign<core::TransformComponent>().translation = editorCamera.getPos();
        }
 
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

        entityViewPanel.setSceneContext(&scene.getScene());
        entityViewPanel.setSelectecEntity(sceneHierarchyPanel.getSelectedEntity());
        
        for (auto panel : panels) {
            panel->renderPanel();
        }

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
            dispatcher.post<core::ViewPortResizeEvent>(static_cast<uint32_t>(vpSize.x), static_cast<uint32_t>(vpSize.y));
        }
        editorCamera.onUpdate(window.getWindow() ,ts);

        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void*>(renderer.displayViewPort())), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        ImGui::PopStyleVar(2);
        // ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void*>(renderer.shadow())), {1024, 1024}, ImVec2(0, 1), ImVec2(1, 0));

        if (sceneHierarchyPanel.getSelectedEntity() != ecs::null) {
            if (scene.getScene().has<core::ScriptComponent>(sceneHierarchyPanel.getSelectedEntity())) {
                auto& scripts = scene.getScene().get<core::ScriptComponent>(sceneHierarchyPanel.getSelectedEntity()).scripts;
                for (auto& kv : scripts) {
                    kv.second.attr("on_imgui_update")();
                }
            }
        }

        // ImGui::Begin("Camera");
        // core::EditorCamera::componentPanel(editorCamera);
        // ImGui::End();

        ImGui::Begin("Settings");
        ImGui::DragFloat("Fps", &settings.fps, 1, 20, 10000);
        ImGui::End();

        core::endFrameImgui(window.getWindow());

        window.updateScreen();
    }

}
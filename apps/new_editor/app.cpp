#include "app.hpp"

#include "controller.hpp"

#include "imgui_utils.hpp"
#include "imgui_panels.hpp"
#include "scene_renderer.hpp"

using namespace core;

App::App() : window("Editor", 1200, 800) {
    initImgui(window.getWindow());
    setupImGuiStyle();
    activeScene = new ecs::Scene{};

    auto camEnt = activeScene->newEntity();
    activeScene->assign<core::Camera>(camEnt);
    activeScene->assign<core::Tag>(camEnt).tag = "Camera";

    activeCamera = camEnt;
    activeControllerEntity = camEnt;

    auto modelEnt = activeScene->newEntity();
    activeScene->assign<core::Model>(modelEnt).loadModelFromPath("../../../models/2.0/Sponza/glTF/Sponza.gltf");
    activeScene->assign<core::Transform>(modelEnt).scale = {.01, .01, .01};
    activeScene->assign<core::Tag>(modelEnt).tag = "Model";

    auto pl = activeScene->newEntity();
    activeScene->assign<core::PointLight>(pl).pos = {1, 1, 1};
    activeScene->assign<core::AmbienceLight>(pl).ambient = {.01, .01, .01};
    activeScene->assign<core::Tag>(pl).tag = "PointLight";

    dispatcher.subscribe<core::SetActiveCameraEvent>([this](const event::Event& e) {
        const SetActiveCameraEvent& event = reinterpret_cast<const SetActiveCameraEvent&>(e);
        this->activeCamera = event.ent;
    });

    dispatcher.subscribe<core::SetActiveKeyboardControllerEntityEvent>([this](const event::Event& e) {
        const SetActiveKeyboardControllerEntityEvent& event = reinterpret_cast<const SetActiveKeyboardControllerEntityEvent&>(e);
        this->activeControllerEntity = event.ent;
    });
}

App::~App() {
    delete activeScene;
    terminateImgui();
}

void App::run() {

    SceneRenderer sceneRenderer(dispatcher);

    core::Controller controller;

    ImVec2 size{100, 100};

    while (!window.shouldClose()) {
        window.pollEvents();

        float dt = ImGui::GetIO().DeltaTime;
        // entity updates

        if (ImGui::IsKeyPressed(ImGuiKey_L)) {
            auto pl = activeScene->newEntity();
            activeScene->assign<core::Tag>(pl).tag = "Point Light";
            auto& pointLight = activeScene->assign<core::PointLight>(pl);
            if (activeCamera != ecs::null) {
                pointLight.pos = activeScene->get<core::Camera>(activeCamera).getPosition();
            }
        }

        // render
        sceneRenderer.render(*activeScene, activeCamera);

        startFrameImgui();

        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None & ~ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar |
                                       ImGuiWindowFlags_NoDocking | 
                                       ImGuiWindowFlags_NoTitleBar |
                                       ImGuiWindowFlags_NoCollapse |
                                    //    ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus |
                                       ImGuiWindowFlags_NoNavFocus |
                                       ImGuiWindowFlags_NoBackground;
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
        // menu
        ImGui::End();
        ImGui::PopStyleVar(2);

        // panels
        frameTime();
        sceneHirarchy(activeScene, dispatcher, selectedEntity);
        selectedEntityComponentViewer(activeScene, dispatcher, selectedEntity);

        // viewport and entity controller
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
        ImGui::SetNextWindowClass(&window_class);
        ImGuiWindowFlags viewPortFlags = ImGuiWindowFlags_NoTitleBar;
        ImGui::Begin("viewport", nullptr, viewPortFlags);
        ImVec2 vpSize = ImGui::GetContentRegionAvail();
        if (vpSize.x != size.x && vpSize.y != size.y) {
            size = vpSize;
            dispatcher.post<core::ViewPortSizeUpdateEvent>(static_cast<uint32_t>(vpSize.x), static_cast<uint32_t>(vpSize.y));
        }
        ImVec2 vpOffset = ImGui::GetWindowPos();
        int mx, my;
        glfwGetWindowPos(window.getWindow(), &mx, &my);
        if (activeControllerEntity != ecs::null) {
            if (activeScene->has<core::Camera>(activeControllerEntity)) {
                auto& camera = activeScene->get<core::Camera>(activeControllerEntity);
                controller.move(window.getWindow(), dt, camera.getPosition(), camera.getOrientation(), reinterpret_cast<glm::vec2&>(vpSize), reinterpret_cast<glm::vec2&>(vpOffset) - glm::vec2{mx, my});
            } else if (activeScene->has<core::Transform>(activeControllerEntity)) {
                auto& transform = activeScene->get<core::Transform>(activeControllerEntity);
                controller.move(window.getWindow(), dt, transform.translation, transform.rotation, reinterpret_cast<glm::vec2&>(vpSize), reinterpret_cast<glm::vec2&>(vpOffset) - glm::vec2{mx, my});
            }
        }
        if (activeCamera != ecs::null) {
            activeScene->get<core::Camera>(activeCamera).update(vpSize.x / vpSize.y);
        }
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void*>(sceneRenderer.displayViewPort())), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        ImGui::PopStyleVar(2);

        ImGui::Begin("Controller Settings");
        core::Controller::componentPanel(controller);
        ImGui::End();

        endFrameImgui(window.getWindow());

        window.updateScreen();
    }
    
}
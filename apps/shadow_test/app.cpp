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

App::App() : window("Editor", 1200, 800) {
    core::initImgui(window.getWindow());
    core::setupImGuiStyle();
    core::myDefaultImGuiStyle();
    float size_pixels = 12;
    ImFont *font = ImGui::GetIO().Fonts->AddFontFromFileTTF("../../../apps/editor/fonts/CooperHewitt-Medium.otf", size_pixels);
    ImGui::GetIO().FontDefault = font;
}

App::~App() {
    core::terminateImgui();
}

void App::run() {

    core::Scene scene;
    event::Dispatcher dispatcher;

    SceneRenderer renderer{dispatcher};

    core::EditorCamera editorCamera;

    ImVec2 size{100, 100};

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

        core::endFrameImgui(window.getWindow());

        window.updateScreen();
    }

}
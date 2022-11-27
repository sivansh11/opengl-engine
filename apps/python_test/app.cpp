#include "app.hpp"

#include "imgui_utils.hpp"
#include "scene_hierarchy_panel.hpp"
#include "events.hpp"
#include "script.hpp"
#include "entity.hpp"
#include "entity_view_panel.hpp"
#include "registered_script_panel.hpp"

#include "pyscript/script_engine.hpp"


App::App() : window("Editor", 640, 420) {
    core::initImgui(window.getWindow());
}

App::~App() {
    core::terminateImgui();
}

void App::run() {
    core::Scene       scene;
    event::Dispatcher dispatcher;
    ecs::EntityID     selectedEnt = scene.getScene().null;

    auto ent = scene.createEntity();
    auto& entScripts = ent.assign<core::ScriptComponent>();
    ent.assign<core::TagComponent>("Entity");

    py::eval_file("../../../apps/python_test/python_scripts/test.py");
    // auto test = core::pyscript::ScriptEngine::getObjects()["Test"];
    // entScripts.scripts["Test"] = test();

    while (!window.shouldClose()) {
        window.pollEvents();

        float ts = ImGui::GetIO().DeltaTime;

        glClear(GL_COLOR_BUFFER_BIT);

        for (auto [ent, script] : scene.getScene().view<core::ScriptComponent>()) {
            for (auto& kv : script.scripts) {
                kv.second.attr("on_update")(ts);
            }
        }

        core::startFrameImgui();

        core::sceneHierarchy(&scene.getScene(), dispatcher, selectedEnt);
        core::entityView(&scene.getScene(), dispatcher, selectedEnt);
        core::registeredScripts();

        ImGui::Text("%.5f ms  %f fps", ImGui::GetIO().DeltaTime * 1000, ImGui::GetIO().Framerate);

        core::endFrameImgui(window.getWindow());

        window.updateScreen();
    }
    
}
#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include "../pyscript/script_engine.hpp"
#include "../entity.hpp"

#include <imgui.h>

#include <unordered_map>
#include <string>

namespace core {

struct Script {
    const char *name;
    py::object *object;
};

struct ScriptComponent {
    std::unordered_map<std::string, py::object> scripts;

    static void componentPanel(ScriptComponent& scriptComponent, event::Dispatcher& dispatcher, core::Scene32& scene, ecs::EntityID ent) {
        ImGui::Selectable("Drop Script Class", true);
        if (ImGui::BeginDragDropTarget()) {
            const ImGuiPayload *payload = ImGui::GetDragDropPayload();
            assert(payload != nullptr);
            if (std::memcmp(payload->DataType, "SCRIPT_CLASS", 13) != 0) {
                ImGui::EndDragDropTarget();
                return;
            }
            Script& script = *reinterpret_cast<Script *>(payload->Data);
            if (scriptComponent.scripts.contains(script.name)) {
                ImGui::EndDragDropTarget();
                return;
            }
            scriptComponent.scripts[script.name] = (*(script.object))(core::Entity(scene, ent));
            ImGui::EndDragDropTarget();
        }
        ImGui::Text("Active Scripts:");
        for (auto& kv : scriptComponent.scripts) {
            ImGui::Text("%s", kv.first.c_str());
        }
    }
};

} // namespace core

#endif
#ifndef REGISTERED_SCRIPT_PANEL_HPP
#define REGISTERED_SCRIPT_PANEL_HPP

#include "panel.hpp"
#include "script.hpp"
#include "pyscript/script_engine.hpp"

namespace core {

class RegisteredScriptsPanel : public Panel {
public:
    RegisteredScriptsPanel() {}

    void renderPanel() {
        if (!m_show) return;
        ImGui::Begin("Registed Scripts Panel");
        if (ImGui::BeginDragDropTarget()) {
            const ImGuiPayload *payload = ImGui::GetDragDropPayload();
            if (std::memcmp(payload->DataType, "PATH", 4) != 0) {
                ImGui::EndDragDropTarget();
                ImGui::End();
                return;
            }
            pyscript::ScriptEngine::exec_script({reinterpret_cast<char *>(payload->Data), payload->DataSize});
            ImGui::EndDragDropTarget();
        }

        auto& pyScripts = pyscript::ScriptEngine::getObjects();

        for (auto& kv : pyScripts) {
            ImGui::Selectable(kv.first.c_str(), true);    
            if (ImGui::BeginDragDropSource()) {
                Script script;
                script.name = kv.first.c_str();
                script.object = &kv.second;
                ImGui::SetDragDropPayload("SCRIPT_CLASS", &script, sizeof(script));
                ImGui::EndDragDropSource();
            }
        }
        ImGui::End();
    }

private:

};

} // namespace core

#endif
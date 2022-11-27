#ifndef SCENE_HIERARCHY_PANEL_HPP
#define SCENE_HIERARCHY_PANEL_HPP

#include "entity.hpp"
#include "events.hpp"
#include "tag.hpp"
#include "transform.hpp"
#include "light.hpp"
#include "model.hpp"
#include "script.hpp"

#include "panel.hpp"

namespace core {

class SceneHierarchyPanel : public  Panel {
public:
    SceneHierarchyPanel(event::Dispatcher& dispatcher) : m_dispatcher(dispatcher) {}

    void renderPanel(core::Scene32 *scene) {
        if (!m_show) return;
        ImGui::Begin("Scene Hierarchy Panel");
        if (!scene) {
            ImGui::End();
            return;
        }
        for (auto ent : scene->view()) 
            drawEntityNode(*scene, ent);

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered()) 
            m_selectedEntity = scene->null;

        ImGuiPopupFlags popupFlags = ImGuiPopupFlags_NoOpenOverItems |
                                        ImGuiPopupFlags_MouseButtonRight;

        if (ImGui::BeginPopupContextWindow(NULL, popupFlags)) {
            if (ImGui::MenuItem("Create Empty Entity")) {
                auto ent = scene->createEntity();
                scene->assign<TagComponent>(ent).tag = "New Entity";
            }
            ImGui::EndPopup();
        }
        ImGui::End();   
        
    }
    
    ecs::EntityID getSelectedEntity() { return m_selectedEntity; }

private:
    void drawEntityNode(core::Scene32& scene, ecs::EntityID ent) {
        auto& tag = scene.get<TagComponent>(ent).tag;

        ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_SpanAvailWidth;
        bool opened = ImGui::TreeNodeEx(tag.c_str(), treeFlags);

        if (ImGui::BeginPopupContextItem(std::to_string(ent).c_str())) {
            if (!scene.has<TransformComponent>(ent) && ImGui::MenuItem("Add Transform")) 
                scene.assign<TransformComponent>(ent);
            if (!scene.has<PointLightComponent>(ent) && ImGui::MenuItem("Add Point Light")) 
                scene.assign<PointLightComponent>(ent);
            if (!scene.has<Model>(ent) && ImGui::MenuItem("Add Model")) 
                scene.assign<Model>(ent);
            if (scene.has<PointLightComponent>(ent) && !scene.has<AmbienceLightComponent>(ent) && ImGui::MenuItem("Add Ambience Light")) 
                scene.assign<AmbienceLightComponent>(ent);
            if (!scene.has<ScriptComponent>(ent) && ImGui::MenuItem("Add Script"))
                scene.assign<ScriptComponent>(ent);

            if (ImGui::MenuItem("Delete Entity")) {
                scene.destroyEntity(ent);
                if (ent == m_selectedEntity) {
                    m_selectedEntity = scene.null;
                }
            }
            ImGui::EndPopup();  
        }

        if (ImGui::IsItemClicked()) 
            m_selectedEntity = ent;

        if (opened) 
            ImGui::TreePop();
    }

private:
    event::Dispatcher& m_dispatcher;
    ecs::EntityID m_selectedEntity{};
};

} // namespace core

#endif
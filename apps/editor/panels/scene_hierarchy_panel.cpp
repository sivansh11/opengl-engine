#include "scene_hierarchy_panel.hpp"

#include "../custom_events.hpp"

SceneHierarchyPanel::SceneHierarchyPanel(event::Dispatcher& dispatcher) : Panel("Scene Hierarchy Panel"), m_dispatcher(dispatcher) {}

void SceneHierarchyPanel::setSceneContext(core::Scene *scene) {
    SceneHierarchyPanel::scene = scene;
}

void SceneHierarchyPanel::onImGuiRender() {
    if (!m_show) return;
    ImGui::Begin("Scene Hierarchy Panel");
    if (!scene) {
        ImGui::End();
        return;
    }
    for (auto ent : scene->group()) 
        drawEntityNode(*scene, ent);

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered()) 
        m_selectedEntity = ecs::null;

    ImGuiPopupFlags popupFlags = ImGuiPopupFlags_NoOpenOverItems |
                                    ImGuiPopupFlags_MouseButtonRight;

    if (ImGui::BeginPopupContextWindow(NULL, popupFlags)) {
        if (ImGui::MenuItem("Create Empty Entity")) {
            auto ent = scene->createEntity();
            scene->assign<core::TagComponent>(ent).tag = "New Entity";
        }
        ImGui::EndPopup();
    }

    // ImGui::InvisibleButton("SceneButton", ImGui::GetContentRegionAvail());
    if (ImGui::BeginDragDropTarget()) {
        const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("FILE_PATH", ImGuiDragDropFlags_AcceptBeforeDelivery);
        if (payload) {
            // (payload->Data)
            std::cout << "Yes\n";
        }

        ImGui::EndDragDropTarget();
    }
    ImGui::End();   
    
}

void SceneHierarchyPanel::drawEntityNode(core::Scene& scene, ecs::EntityID ent) {
    auto& tag = scene.get<core::TagComponent>(ent).tag;

    ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_SpanAvailWidth;
    bool opened;
    if (tag == "") {
        opened = ImGui::TreeNodeEx(" ", treeFlags);
    } else {
        opened = ImGui::TreeNodeEx(tag.c_str(), treeFlags);
    }

    if (ImGui::BeginPopupContextItem(std::to_string(ent).c_str())) {
        if (!scene.has<core::TransformComponent>(ent) && ImGui::MenuItem("Add Transform")) 
            scene.assign<core::TransformComponent>(ent);
        if (!scene.has<core::PointLightComponent>(ent) && ImGui::MenuItem("Add Light Data")) 
            scene.assign<core::PointLightComponent>(ent);
        if (!scene.has<renderer::Model>(ent) && ImGui::MenuItem("Add Model")) 
            scene.assign<renderer::Model>(ent);
        if (!scene.has<core::DirectionalLightComponent>(ent) && ImGui::MenuItem("Add Directional Light")) 
            scene.assign<core::DirectionalLightComponent>(ent);

        if (ImGui::MenuItem("Delete Entity")) {
            scene.destroyEntity(ent);
            if (ent == m_selectedEntity) {
                m_selectedEntity = ecs::null;
            }
            m_dispatcher.post<EntityDeleteEvent>(m_selectedEntity);
        }
        ImGui::EndPopup();  
    }

    if (ImGui::IsItemClicked()) 
        m_dispatcher.post<EntityDeleteEvent>(ent);

    if (opened) 
        ImGui::TreePop();
}

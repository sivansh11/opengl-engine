#include "entity_view_panel.hpp"

namespace core {

EntityViewPanel::EntityViewPanel(event::Dispatcher& dispatcher) : Panel("Entity View Panel"),  m_dispatcher(dispatcher) {}

void EntityViewPanel::renderPanel() {
    if (!scene) return;
    if (ent == ecs::null) return; 
    if (!m_show) return;

    ImGui::Begin("Entity View Panel");

    if (!scene || ent == ecs::null) {
        ImGui::End();
        return;
    }

    ImGui::Text("Tag Component");
    TagComponent::componentPanel(scene->get<TagComponent>(ent));
    ImGui::Separator();

    if (scene->has<TransformComponent>(ent)) {
        utils::Section("Transform Component");
        TransformComponent::componentPanel(scene->get<TransformComponent>(ent), m_dispatcher, ent);
    }
    if (scene->has<PointLightComponent>(ent)) {
        utils::Section("Point Light Component");
        PointLightComponent::componentPanel(scene->get<PointLightComponent>(ent), m_dispatcher, ent);
    }
    if (scene->has<AmbienceLightComponent>(ent)) {
        utils::Section("Ambience Light Component");
        AmbienceLightComponent::componentPanel(scene->get<AmbienceLightComponent>(ent), m_dispatcher, ent);
    }
    if (scene->has<CameraComponent>(ent)) {
        utils::Section("Camera Component");
        CameraComponent::componentPanel(scene->get<CameraComponent>(ent), m_dispatcher, ent);
    }
    if (scene->has<Model>(ent)) {
        utils::Section("Model Component");
        Model::componentPanel(scene->get<Model>(ent), m_dispatcher, ent);
    }
    if (scene->has<ScriptComponent>(ent)) {
        utils::Section("Script Component");
        ScriptComponent::componentPanel(scene->get<ScriptComponent>(ent), m_dispatcher, *scene, ent);
    }

    ImGui::End();
}

} // namespace core

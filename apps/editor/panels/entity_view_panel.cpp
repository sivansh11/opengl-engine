#include "entity_view_panel.hpp"

#include "../custom_events.hpp"

EntityViewPanel::EntityViewPanel(event::Dispatcher& dispatcher) : Panel("Entity View Panel"),  m_dispatcher(dispatcher) {
    m_dispatcher.subscribe<EntityDeleteEvent>([this](const event::Event& e) {
        const EntityDeleteEvent& event = reinterpret_cast<const EntityDeleteEvent&>(e);
        this->setSelectecEntity(event.ent);
    });

}

void EntityViewPanel::onImGuiRender() {
    if (!m_show) return;

    ImGui::Begin("Entity View Panel");
    
    if (!scene) {
        ImGui::End();
        return;
    }

    if (ent == ecs::null) {
        ImGui::End();
        return;
    } 

    if (!scene || ent == ecs::null) {
        ImGui::End();
        return;
    }

    ImGui::Text("Tag Component");
    core::TagComponent::componentPanel(scene->get<core::TagComponent>(ent));
    ImGui::Separator();

    if (scene->has<core::TransformComponent>(ent)) {
        utils::Section section("Transform Component");
        core::TransformComponent::componentPanel(scene->get<core::TransformComponent>(ent), m_dispatcher, ent);
    }
    if (scene->has<core::PointLightComponent>(ent)) {
        utils::Section section("Point Light Component");
        core::PointLightComponent::componentPanel(scene->get<core::PointLightComponent>(ent), m_dispatcher, ent);
    }
    
    if (scene->has<renderer::Model>(ent)) {
        utils::Section section("Model Component");
        renderer::Model::componentPanel(scene->get<renderer::Model>(ent), m_dispatcher, ent);
    }

    if (scene->has<core::DirectionalLightComponent>(ent)) {
        utils::Section section("Directional Light Component");
        core::DirectionalLightComponent::componentPanel(scene->get<core::DirectionalLightComponent>(ent), m_dispatcher, ent);
    }
    
    ImGui::End();
}

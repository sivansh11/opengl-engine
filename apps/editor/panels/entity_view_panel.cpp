#include "entity_view_panel.hpp"

#include "../custom_events.hpp"

EntityViewPanel::EntityViewPanel(event::Dispatcher& dispatcher) : Panel("Entity View Panel"),  m_dispatcher(dispatcher) {
    m_dispatcher.subscribe<EntityDeleteEvent>([this](const event::Event& e) {
        const EntityDeleteEvent& event = reinterpret_cast<const EntityDeleteEvent&>(e);
        this->setSelectecEntity(event.ent);
    });

}

void EntityViewPanel::renderPanel() {
    if (!scene) return;
    if (!m_show) return;

    ImGui::Begin("Entity View Panel");

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
        utils::Section("Transform Component");
        core::TransformComponent::componentPanel(scene->get<core::TransformComponent>(ent), m_dispatcher, ent);
    }
    if (scene->has<core::LightData>(ent)) {
        utils::Section("Point Light Component");
        core::LightData::componentPanel(scene->get<core::LightData>(ent), m_dispatcher, ent);
    }
    
    if (scene->has<renderer::Model>(ent)) {
        utils::Section("Model Component");
        renderer::Model::componentPanel(scene->get<renderer::Model>(ent), m_dispatcher, ent);
    }
    
    ImGui::End();
}

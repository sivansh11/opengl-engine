#ifndef ENTITY_VIEW_PANEL_HPP
#define ENTITY_VIEW_PANEL_HPP

#include "entity.hpp"
#include "events.hpp"
#include "tag.hpp"
#include "transform.hpp"
#include "light.hpp"
#include "model.hpp"
#include "script.hpp"
#include "camera.hpp"

#include "panel.hpp"

namespace core {

namespace utils {

struct Section {
    Section(const char *label) {
        ImGui::Text("%s", label);
    } 
    ~Section() {
        ImGui::Separator();
    }
};

} // namespace utils

class EntityViewPanel : public Panel {
public:
    EntityViewPanel(event::Dispatcher& dispatcher) : m_dispatcher(dispatcher) {}

    void renderPanel(core::Scene32 *scene, ecs::EntityID ent) {
        if (!m_show) return;

        ImGui::Begin("Entity View Panel");

        if (!scene || ent == scene->null) {
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


private:
    event::Dispatcher& m_dispatcher;
};

} // namespace core

#endif
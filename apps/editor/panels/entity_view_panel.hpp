#ifndef ENTITY_VIEW_PANEL_HPP
#define ENTITY_VIEW_PANEL_HPP

#include "core/entity.hpp"
#include "core/events.hpp"
#include "core/components.hpp"
#include "renderer/model.hpp"

#include "core/panel.hpp"

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

class EntityViewPanel : public core::Panel {
public:
    EntityViewPanel(event::Dispatcher& dispatcher);

    void onImGuiRender() override;
    void setSceneContext(core::Scene *scene) { EntityViewPanel::scene = scene; }
    void setSelectecEntity(ecs::EntityID ent) { EntityViewPanel::ent = ent; }

private:
    core::Scene *scene = nullptr;
    ecs::EntityID ent = ecs::null;
    event::Dispatcher& m_dispatcher;
};

#endif
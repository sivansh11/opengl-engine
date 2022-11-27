#ifndef ENTITY_VIEW_PANEL_HPP
#define ENTITY_VIEW_PANEL_HPP

#include "../entity.hpp"
#include "../events.hpp"
#include "../components/tag.hpp"
#include "../components/transform.hpp"
#include "../components/light.hpp"
#include "../model.hpp"
#include "../components/script.hpp"
#include "../components/camera.hpp"

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
    EntityViewPanel(event::Dispatcher& dispatcher);

    void renderPanel(core::Scene32 *scene, ecs::EntityID ent);

private:
    event::Dispatcher& m_dispatcher;
};

} // namespace core

#endif
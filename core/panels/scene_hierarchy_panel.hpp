#ifndef SCENE_HIERARCHY_PANEL_HPP
#define SCENE_HIERARCHY_PANEL_HPP

#include "../entity.hpp"
#include "../events.hpp"
#include "../components/tag.hpp"
#include "../components/transform.hpp"
#include "../components/light.hpp"
#include "../model.hpp"
#include "../components/script.hpp"

#include "panel.hpp"

namespace core {

class SceneHierarchyPanel : public  Panel {
public:
    SceneHierarchyPanel(event::Dispatcher& dispatcher);

    void renderPanel() override;

    void setSceneContext(core::Scene32 *scene);
    
    ecs::EntityID getSelectedEntity() { return m_selectedEntity; }

private:
    void drawEntityNode(core::Scene32& scene, ecs::EntityID ent);

private:
    core::Scene32 *scene = nullptr;
    event::Dispatcher& m_dispatcher;
    ecs::EntityID m_selectedEntity{};
};

} // namespace core

#endif
#ifndef SCENE_HIERARCHY_PANEL_HPP
#define SCENE_HIERARCHY_PANEL_HPP

#include "core/entity.hpp"
#include "core/events.hpp"
#include "core/components.hpp"
#include "renderer/model.hpp"

#include "panel.hpp"

class SceneHierarchyPanel : public  Panel {
public:
    SceneHierarchyPanel(event::Dispatcher& dispatcher);

    void renderPanel() override;

    void setSceneContext(core::Scene *scene);
    
    ecs::EntityID getSelectedEntity() { return m_selectedEntity; }

private:
    void drawEntityNode(core::Scene& scene, ecs::EntityID ent);

private:
    core::Scene *scene = nullptr;
    event::Dispatcher& m_dispatcher;
    ecs::EntityID m_selectedEntity = ecs::null;
};

#endif
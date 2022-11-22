#ifndef EVENT_TYPES_HPP
#define EVENT_TYPES_HPP

#include "events.hpp"
#include "ecs.hpp"

namespace core {

struct ViewPortSizeUpdateEvent : public event::Event {
    ViewPortSizeUpdateEvent(uint32_t width, uint32_t height) : width(width), height(height) {}
    uint32_t width, height;
};

struct SetActiveKeyboardControllerEntityEvent : public event::Event {
    SetActiveKeyboardControllerEntityEvent(ecs::EntityID ent) : ent(ent) {}
    ecs::EntityID ent;
};

struct SetActiveCameraEvent : public event::Event {
    SetActiveCameraEvent(ecs::EntityID ent) : ent(ent) {}
    ecs::EntityID ent;
};

} // namespace core

#endif
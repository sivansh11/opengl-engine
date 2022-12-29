#ifndef CUSTOM_EVENTS_HPP
#define CUSTOM_EVENTS_HPP

#include "core/events.hpp"

#include <iostream>

struct ViewPortResizeEvent : public event::Event {
    ViewPortResizeEvent(uint32_t width, uint32_t height) : width(width), height(height) {}
    uint32_t width, height;
};

struct EntityDeleteEvent : public event::Event {
    EntityDeleteEvent(ecs::EntityID ent) : ent(ent) {}
    ecs::EntityID ent;
};

#endif
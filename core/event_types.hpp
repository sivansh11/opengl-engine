#ifndef EVENT_TYPES_HPP
#define EVENT_TYPES_HPP

#include "events.hpp"
#include "ecs.hpp"

namespace core {

struct ViewPortResizeEvent : public event::Event {
    ViewPortResizeEvent(uint32_t width, uint32_t height) : width(width), height(height) {}
    uint32_t width, height;
};

} // namespace core

#endif
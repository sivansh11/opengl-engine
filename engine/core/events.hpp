#ifndef CORE_EVENTS_HPP
#define CORE_EVENTS_HPP

#include "event.hpp"
#include <iostream>

namespace core {

struct ViewPortResizeEvent : public event::Event {
    ViewPortResizeEvent(uint32_t width, uint32_t height) : width(width), height(height) {}
    uint32_t width, height;
};

} // namespace core

#endif
#ifndef CUSTOM_EVENTS_HPP
#define CUSTOM_EVENTS_HPP

#include "core/events.hpp"

#include <iostream>

struct ViewPortResizeEvent : public event::Event {
    ViewPortResizeEvent(uint32_t width, uint32_t height) : width(width), height(height) {}
    uint32_t width, height;
};

struct ShadowMapResizeEvent : public event::Event {
    ShadowMapResizeEvent(uint32_t width, uint32_t height) : width(width), height(height) {}
    uint32_t width, height;
};

#endif
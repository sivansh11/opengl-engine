#ifndef PANELS_HPP
#define PANELS_HPP

#include "layer.hpp"

#include <imgui.h>

#include <string>

namespace core {

struct Panel : core::Layer {
    Panel(const std::string& panelName);
    bool m_show{true};
    void onImGuiRender() override = 0;
    bool& getShow(); 
};

} // namespace core

#endif
#ifndef PANEL_HPP
#define PANEL_HPP

#include "imgui.h"

#include <string>

namespace core {

struct Panel {
    Panel(const char *panelName) : m_panelName(panelName) {}
    bool m_show{true};
    std::string m_panelName;
    const std::string& getName() const { return m_panelName; }
    bool& getShow() { return m_show; }
    virtual void renderPanel() = 0;
};

} // namespace core

#endif
#ifndef CORE_PANEL_HPP
#define CORE_PANEL_HPP

#include "imgui_utils.hpp"

#include <string>

namespace core {

class BasePanel {
public:
    BasePanel(const std::string& name, bool show = true) : m_name(name), show(show) {}
    void uiPanel() {
        if (!show) return;
        ImGui::Begin(m_name.c_str(), &show);
        UI();
        ImGui::End();
    }

    virtual void UI() {}

    bool show;
    std::string m_name; 
};

} // namespace core

#endif
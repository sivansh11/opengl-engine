#ifndef SETTINGS_PANEL_HPP
#define SETTINGS_PANEL_HPP

#include "core/panel.hpp"

#include "core/window.hpp"

class SettingsPanel : public core::Panel {
public:
    SettingsPanel(core::Window& window);

    void onImGuiRender() override;

    float getFPS() { return m_targetFPS; }
private:
    core::Window& m_window;
    float m_targetFPS = 60;
};

#endif
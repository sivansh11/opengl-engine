#include "settings_panel.hpp"

SettingsPanel::SettingsPanel(core::Window& window) : Panel("Settings Panel"), m_window(window) {

}

void SettingsPanel::onImGuiRender() {
    if (!m_show) return;

    ImGui::Begin("Settings");

    ImGui::DragFloat("FPS", &m_targetFPS, 1);

    ImGui::End();
}
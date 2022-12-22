#include "editor_color_picker_panel.hpp"

namespace core {

EditorColorPickerPanel::EditorColorPickerPanel() : Panel("Editor Color Picker Panel") { 
    m_show = false;    
}

void EditorColorPickerPanel::renderPanel() {
    if (!m_show) return;
    ImGui::Begin("Editor Color Picker");
    ImGui::ColorPicker3("color_for_text", reinterpret_cast<float *>(&color_for_text));
    ImGui::ColorPicker3("color_for_head", reinterpret_cast<float *>(&color_for_head));
    ImGui::ColorPicker3("color_for_area", reinterpret_cast<float *>(&color_for_area));
    ImGui::ColorPicker3("color_for_body", reinterpret_cast<float *>(&color_for_body));
    ImGui::ColorPicker3("color_for_pops", reinterpret_cast<float *>(&color_for_pops));
    ImGui::ColorPicker3("color_for_tabs", reinterpret_cast<float *>(&color_for_tabs));
    ImGui::End();
    easyThemingImGui(color_for_text, color_for_head, color_for_area, color_for_body, color_for_pops, color_for_tabs);
}

} // namespace core

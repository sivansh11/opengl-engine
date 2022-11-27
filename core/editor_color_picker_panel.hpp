#ifndef EDITOR_COLOR_PICKER_PANEL_HPP
#define EDITOR_COLOR_PICKER_PANEL_HPP

#include "panel.hpp"
#include "imgui_utils.hpp"

namespace core {

class EditorColorPickerPanel : public Panel {
public:
    EditorColorPickerPanel() { 
        m_show = false; 
        
    }

    void renderPanel() {
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

private:
    ImVec3 color_for_text = ImVec3(255.f / 255.f, 255.f / 255.f, 255.f / 255.f);
    ImVec3 color_for_head = ImVec3(41.f / 255.f, 128.f / 255.f, 185.f / 255.f);
    ImVec3 color_for_area = ImVec3(57.f / 255.f, 79.f / 255.f, 105.f / 255.f);
    ImVec3 color_for_body = ImVec3(44.f / 255.f, 62.f / 255.f, 80.f / 255.f);
    ImVec3 color_for_pops = ImVec3(33.f / 255.f, 46.f / 255.f, 60.f / 255.f);
    ImVec3 color_for_tabs = ImVec3(33.f / 255.f, 46.f / 255.f, 60.f / 255.f);
};

} // namespace core

#endif
#ifndef EDITOR_COLOR_PICKER_PANEL_HPP
#define EDITOR_COLOR_PICKER_PANEL_HPP

#include "panel.hpp"
#include "core/imgui_utils.hpp"

class EditorColorPickerPanel : public Panel {
public:
    EditorColorPickerPanel();

    void renderPanel() override;
private:
    core::ImVec3 color_for_text = core::ImVec3(255.f / 255.f, 255.f / 255.f, 255.f / 255.f);
    core::ImVec3 color_for_head = core::ImVec3(41.f / 255.f, 128.f / 255.f, 185.f / 255.f);
    core::ImVec3 color_for_area = core::ImVec3(57.f / 255.f, 79.f / 255.f, 105.f / 255.f);
    core::ImVec3 color_for_body = core::ImVec3(44.f / 255.f, 62.f / 255.f, 80.f / 255.f);
    core::ImVec3 color_for_pops = core::ImVec3(33.f / 255.f, 46.f / 255.f, 60.f / 255.f);
    core::ImVec3 color_for_tabs = core::ImVec3(33.f / 255.f, 46.f / 255.f, 60.f / 255.f);
};

#endif
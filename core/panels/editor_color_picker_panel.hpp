#ifndef EDITOR_COLOR_PICKER_PANEL_HPP
#define EDITOR_COLOR_PICKER_PANEL_HPP

#include "panel.hpp"
#include "../imgui_utils.hpp"

namespace core {

class EditorColorPickerPanel : public Panel {
public:
    EditorColorPickerPanel();

    void renderPanel() override;
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
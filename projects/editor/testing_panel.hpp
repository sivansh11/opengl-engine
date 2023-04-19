#ifndef TESTING_PANEL_HPP
#define TESTING_PANEL_HPP

#include "core/panel.hpp"

class ViewPanel : public core::BasePanel {
public:
    ViewPanel() : BasePanel("View Panel") {}

    void UI() override {
        static int currentItem = 1;
        const char *items[] = {
            "finalImage",
            "vxgiFinalImage",
            "voxelVisual",
        };
        ImGui::ListBox("Views", &currentItem, items, 3);
        selectedImage = items[currentItem];
    }
    std::string selectedImage = "vxgiFinalImage";

private:

};

#endif
#ifndef TESTING_PANEL_HPP
#define TESTING_PANEL_HPP

#include "core/panel.hpp"

#include <vector>
#include <string>

class ViewPanel : public core::BasePanel {
public:
    ViewPanel() : BasePanel("View Panel") {}

    void addItem(const std::string& item) {
        items.push_back(item);
    }

    void UI() override {
        std::vector<const char *> actualItems;
        actualItems.reserve(items.size());
        for (auto& item : items) actualItems.push_back(item.data());
        ImGui::ListBox("Views", &currentItem, actualItems.data(), items.size());
    }

    std::string getCurrentImage() { return items[currentItem]; }

    int currentItem = 0;
    std::vector<std::string> items;

private:

};

#endif
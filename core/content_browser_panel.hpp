#ifndef CONTENT_BROWSER_PANEL_HPP
#define CONTENT_BROWSER_PANEL_HPP

#include "panel.hpp"

#include <iostream>
#include <filesystem>

namespace core {

class ContentBrowserPanel : public Panel {
public:
    ContentBrowserPanel(std::filesystem::path initialPath) : m_path(initialPath)  {}

    void renderPanel() {
        if (!m_show) return;

        ImGui::Begin("Content Browser Panel");
    
        showTreeNode(m_path, true);
        
        ImGui::End();
    }

private:

    void showTreeNode(const std::filesystem::path& currentPath, bool isDir) {
        bool opened = ImGui::TreeNodeEx(currentPath.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth);

        if (!isDir) {
            if (ImGui::BeginDragDropSource()) {
                static std::filesystem::path s_selectedPath;
                s_selectedPath = currentPath;
                ImGui::SetDragDropPayload("PATH", s_selectedPath.c_str(), s_selectedPath.string().size());
                ImGui::EndDragDropSource();
            }
            if (opened) 
                ImGui::TreePop();
            return;
        }

        if (opened) {
            for (auto& directoryEntry : std::filesystem::directory_iterator(currentPath)) {
                showTreeNode(directoryEntry.path(), directoryEntry.is_directory());
            }
            ImGui::TreePop();
        }
    }

private:
    std::filesystem::path m_path;
};

} // namespace core

#endif
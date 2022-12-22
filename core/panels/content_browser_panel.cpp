#include "content_browser_panel.hpp"

namespace core {

ContentBrowserPanel::ContentBrowserPanel(std::filesystem::path initialPath) 
  : Panel("Content Browser Panel"),
    m_path(initialPath),
    m_directoryIcon("../../../asset/DirectoryIcon.png"),
    m_fileIcon("../../../asset/FileIcon.png") {

}

void ContentBrowserPanel::renderPanel() {
    if (!m_show) return;

    // ImGui::Begin("Content Browser Panel");

    // showTreeNode(m_path, true);
    
    // ImGui::End();

    ImGui::Begin("Content Browser Panel");

    if (ImGui::Button("<-")) {
        m_path = m_path.parent_path();
    }
    
    static float padding = 9.f;
    static float thumbnailSize = 70.f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1) 
        columnCount = 1;
    
    ImGui::Columns(columnCount, 0, false);

    for (auto& directorEntry : std::filesystem::directory_iterator(m_path)) {
        const auto& path = directorEntry.path();
        std::string filenameString = path.filename().string();

        ImGui::PushID(filenameString.c_str());
        gfx::Texture2D *icon = directorEntry.is_directory() ? &m_directoryIcon : &m_fileIcon;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::ImageButton(static_cast<ImTextureID>(reinterpret_cast<void*>(icon->getID())), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});

        if (!directorEntry.is_directory() && ImGui::BeginDragDropSource()) {
            static std::filesystem::path s_selectedPath;
            s_selectedPath = directorEntry.path();
            ImGui::SetDragDropPayload("PATH", s_selectedPath.c_str(), s_selectedPath.string().size());
            ImGui::EndDragDropSource();
        }

        ImGui::PopStyleColor();

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            if (directorEntry.is_directory()) 
                m_path /= path.filename();
        }

        ImGui::TextWrapped("%s", filenameString.c_str());

        ImGui::NextColumn();

        ImGui::PopID();
    }

    ImGui::Columns(1);

    // ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
    // ImGui::SliderFloat("Padding", &padding, 9, 32);

    ImGui::End();
}

// void ContentBrowserPanel::showTreeNode(const std::filesystem::path& currentPath, bool isDir) {
//     bool opened = ImGui::TreeNodeEx(currentPath.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth);

//     if (!isDir) {
//         if (ImGui::BeginDragDropSource()) {
//             static std::filesystem::path s_selectedPath;
//             s_selectedPath = currentPath;
//             ImGui::SetDragDropPayload("PATH", s_selectedPath.c_str(), s_selectedPath.string().size());
//             ImGui::EndDragDropSource();
//         }
//         if (opened) 
//             ImGui::TreePop();
//         return;
//     }

//     if (opened) {
//         for (auto& directoryEntry : std::filesystem::directory_iterator(currentPath)) {
//             showTreeNode(directoryEntry.path(), directoryEntry.is_directory());
//         }
//         ImGui::TreePop();
//     }
// }

} // namespace core

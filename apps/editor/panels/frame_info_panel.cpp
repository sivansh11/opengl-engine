#include "frame_info_panel.hpp"

FrameInfoPanel::FrameInfoPanel() : Panel("Frame Info Panel") {}

void FrameInfoPanel::onImGuiRender() {
    if (!m_show) return;
    ImGui::Begin("Frame Info");
    ImGui::Text("%.5f ms  %.5f fps", ImGui::GetIO().DeltaTime * 1000, ImGui::GetIO().Framerate);
    ImGui::End();
}


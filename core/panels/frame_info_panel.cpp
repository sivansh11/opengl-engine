#include "frame_info_panel.hpp"

namespace core {

FrameInfoPanel::FrameInfoPanel() {}

void FrameInfoPanel::renderPanel() {
    if (!m_show) return;
    ImGui::Begin("Frame Info");
    ImGui::Text("%.5f ms  %.5f fps", ImGui::GetIO().DeltaTime * 1000, ImGui::GetIO().Framerate);
    ImGui::End();
}


} // namespace core

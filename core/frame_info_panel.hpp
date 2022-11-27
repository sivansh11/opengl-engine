#ifndef FRAME_INFO_PANEL_HPP
#define FRAME_INFO_PANEL_HPP

#include "panel.hpp"

namespace core {

class FrameInfoPanel : public Panel {
public:
    FrameInfoPanel() {}

    void renderPanel() {
        if (!m_show) return;
        ImGui::Begin("Frame Info");
        ImGui::Text("%.5f ms  %.5f fps", ImGui::GetIO().DeltaTime * 1000, ImGui::GetIO().Framerate);
        ImGui::End();
    }

private:

};

} // namespace core

#endif

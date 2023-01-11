#ifndef FRAME_INFO_PANEL_HPP
#define FRAME_INFO_PANEL_HPP

#include "core/panel.hpp"

class FrameInfoPanel : public core::Panel {
public:
    FrameInfoPanel();

    void onImGuiRender() override;

private:

};

#endif

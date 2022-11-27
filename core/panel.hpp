#ifndef PANEL_HPP
#define PANEL_HPP

#include "imgui.h"

namespace core {

struct Panel {
    bool m_show{true};

    bool& getShow() { return m_show; }
};

} // namespace core

#endif
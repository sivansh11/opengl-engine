#include "panel.hpp"

namespace core {

Panel::Panel(const std::string& panelName) : Layer(panelName) {

}

bool& Panel::getShow() { return m_show; }

} // namespace core

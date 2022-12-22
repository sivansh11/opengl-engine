#ifndef REGISTERED_SCRIPT_PANEL_HPP
#define REGISTERED_SCRIPT_PANEL_HPP

#include "panel.hpp"
#include "../components/script.hpp"
#include "../pyscript/script_engine.hpp"

namespace core {

class RegisteredScriptsPanel : public Panel {
public:
    RegisteredScriptsPanel();

    void renderPanel() override;

private:

};

} // namespace core

#endif
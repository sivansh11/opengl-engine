#include "app.hpp"

#include "pyscript/script_engine.hpp"

#include <iostream>

#include <pybind11/embed.h>

namespace py = pybind11;

int main() {

    if (!core::pyscript::ScriptEngine::init()) {
        std::cout << "Failed to initialise script engine!";
        return 1;
    }

    App *app = new App();

    app->run();

    delete app;
    
    core::pyscript::ScriptEngine::free();

    return 0;
}
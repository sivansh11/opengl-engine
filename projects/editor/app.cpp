#include "app.hpp"

#include <renderer/mesh.hpp>

#include "core/imgui_utils.hpp"

#include <entt/entt.hpp>

#include <iostream>

#define ASSETS_PATH std::string("../../../assets")

App::App() : window("Editor", 800, 600) {
    core::initImgui(window);
    // core::setupImGuiStyle();
    // core::myDefaultImGuiStyle();
}

App::~App() {
    core::terminateImgui();
}

void App::run() {

    

    double lastTime = glfwGetTime();

    while (!window.shouldClose()) {
        window.pollEvents();

        double currentTime = glfwGetTime(); 
        float dt = currentTime - lastTime;
        lastTime = currentTime;

        core::startFrameImgui();

        core::endFrameImgui(window);
        
        window.updateScreen();
    }

}
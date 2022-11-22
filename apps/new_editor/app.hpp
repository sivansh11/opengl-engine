#ifndef APP_HPP
#define APP_HPP

#include "window.hpp"
#include "ecs.hpp"
#include "events.hpp"

class App {
public:
    App();
    ~App();

    void run();
    
private:
    core::Window window;
    ecs::Scene *activeScene;
    event::Dispatcher dispatcher;

    ecs::EntityID activeCamera = ecs::null;
    ecs::EntityID activeControllerEntity = ecs::null;
    ecs::EntityID selectedEntity = ecs::null;
};

#endif
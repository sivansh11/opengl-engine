#ifndef APP_HPP
#define APP_HPP

#include "window.hpp"

class App {
public:
    App();
    ~App();

    void run();

private:
    core::Window window;
};

#endif
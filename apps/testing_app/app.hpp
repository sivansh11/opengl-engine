#ifndef APP_HPP
#define APP_HPP

#include "window.hpp"

namespace app {

class App {
public:
    App();
    ~App();

    void run();

private:
    core::Window window;
};

} // namespace app

#endif
#include "app.hpp"

#include <iostream>

int main() {
    App *app = new App();
    try {
        app->run();
    } catch (std::exception& e) {
        std::cout << e.what() << '\n';
    }
    delete app;

    return 0;
}
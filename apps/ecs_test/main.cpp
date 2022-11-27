#include <iostream>

#include "entity.hpp"

int main() {

    core::Scene scene;
    
    auto ent = scene.createEntity();
    auto i = ent.assign<int>(6);

    std::cout << ent.get<int>() << '\n';

    return 0;
}
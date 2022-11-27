#include "entity.hpp"

namespace core {

Scene::Scene() {

}

Scene::~Scene() {

}

Entity Scene::createEntity() {
    return Entity{this->scene, scene.createEntity()};
}

} // namespace core

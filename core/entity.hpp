#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "ecs.hpp"

#include "components/transform.hpp"

#include <memory>

namespace core {

using Scene32 = ecs::Scene<32>;

class Entity;

class Scene;

class Scene {
public:
    Scene();
    ~Scene();

    Scene32& getScene() {
        return scene;
    }

    Entity createEntity();

friend class Entity;

private:
    Scene32 scene;
};

class Entity {
public:
    Entity(const Entity& entity) : scene(entity.scene), id(entity.id)  {}

    ecs::EntityID getID() { return id; }

    template <typename T, typename... Args>
    T& assign(Args&&... args) {
        return scene.assign<T>(id, std::forward<Args>(args)...);
    }

    template <typename T>
    T& get() {
        return scene.get<T>(id);
    }

    template <typename T>
    bool has() {
        return scene.has<T>(id);
    }

    Entity(Scene32& scene, ecs::EntityID id) : scene(scene), id(id) {}

friend class Scene;

private:    
    Scene32& scene;
    ecs::EntityID id;
};

} // namespace core

#endif 
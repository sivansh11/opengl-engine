#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "event_types.hpp"
#include "ecs.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "imgui.h"

namespace core {

struct TransformComponent {
    glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::vec3& translation)
        : translation(translation) {}

    glm::mat4 mat4() const
    {
        glm::mat4 rot = glm::toMat4(glm::quat(rotation));

        return glm::translate(glm::mat4(1.0f), translation)
            * rot
            * glm::scale(glm::mat4(1.0f), scale);
    }
    static void componentPanel(TransformComponent& transform, event::Dispatcher& dispatcher, ecs::EntityID ent) {
        ImGui::DragFloat3("Translation", reinterpret_cast<float*>(&transform.translation), 0.01f);
        ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&transform.rotation), 0.01f);
        ImGui::DragFloat3("Scale", reinterpret_cast<float*>(&transform.scale), 0.01f);
    }
};

} // namespace core

#endif
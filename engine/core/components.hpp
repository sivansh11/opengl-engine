#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "ecs.hpp"
#include "events.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <memory>

namespace core {

struct TagComponent {
    std::string tag;

    static void componentPanel(TagComponent& tag) {
        ImGui::InputText("Tag", &tag.tag);
    }
};

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

class CameraComponent {
public:
    CameraComponent() {}
    ~CameraComponent() {}

    void update(float aspectRation) {
        m_projection = glm::perspective(glm::radians(m_fov), aspectRation, m_near, m_far);
    }

    const glm::mat4& getProjection() const {
        return m_projection;
    }

    static void componentPanel(CameraComponent& camera, event::Dispatcher& dispatcher, ecs::EntityID ent) {
        ImGui::DragFloat("FOV", &camera.m_fov, 0.01f);
        ImGui::DragFloat("Far Plane", &camera.m_far, 0.01f);
        ImGui::DragFloat("Near Plane", &camera.m_near, 0.01f);
    }

private:
    glm::mat4 m_projection{1.0f};
    float m_fov{45.0f};
    float m_far{1000.f}, m_near{0.1f};
};

struct LightData {
    alignas(16) glm::vec3 pos;
    alignas(16) glm::vec3 color{1, 1, 1};
    alignas(16) glm::vec3 term{.1, .1, .1};
    alignas(16) glm::vec3 ambience;

    static void componentPanel(LightData& lightData, event::Dispatcher& dispatcher, ecs::EntityID ent) {
        ImGui::DragFloat3("position", reinterpret_cast<float *>(&lightData.pos), 0.01);        
        ImGui::DragFloat3("color", reinterpret_cast<float *>(&lightData.color), 0.01);        
        ImGui::DragFloat3("term", reinterpret_cast<float *>(&lightData.term), 0.01);        
        ImGui::DragFloat3("ambience", reinterpret_cast<float *>(&lightData.ambience), 0.01);        
    }
};

} // namespace core

#endif
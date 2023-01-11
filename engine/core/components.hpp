#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "entity.hpp"
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

    const glm::mat4& getView() const { 
        return m_view; 
    }

    const glm::vec3& getPos() const { return m_pos; }

    static void componentPanel(CameraComponent& camera, event::Dispatcher& dispatcher, ecs::EntityID ent) {
        ImGui::DragFloat("FOV", &camera.m_fov, 0.01f);
        ImGui::DragFloat("Far Plane", &camera.m_far, 0.01f);
        ImGui::DragFloat("Near Plane", &camera.m_near, 0.01f);
    }

    glm::vec3 m_pos{-1, 1, 0};
    glm::mat4 m_projection{1.0f};
    glm::mat4 m_view{1};
    float m_fov{45.0f};
    float m_far{1000.f}, m_near{0.1f};
};

struct PointLightComponent {
    alignas(16) glm::vec3 pos;
    alignas(16) glm::vec3 color{1, 1, 1};
    alignas(16) glm::vec3 term{.1, .1, .1};
    // alignas(16) glm::vec3 ambience;

    static void componentPanel(PointLightComponent& lightData, event::Dispatcher& dispatcher, ecs::EntityID ent) {
        ImGui::DragFloat3("position", reinterpret_cast<float *>(&lightData.pos), 0.01);        
        ImGui::DragFloat3("color", reinterpret_cast<float *>(&lightData.color), 0.01);        
        ImGui::DragFloat3("term", reinterpret_cast<float *>(&lightData.term), 0.01);        
        // ImGui::DragFloat3("ambience", reinterpret_cast<float *>(&lightData.ambience), 0.01);        
    }
};

struct DirectionalLightComponent {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 ambience;

    float multiplier = 1;
    float orthoProj = 10;
    float far = 7.5;
    float near = 1;

    static void componentPanel(DirectionalLightComponent& directionalLight, event::Dispatcher& dispatcher, ecs::EntityID ent) {
        ImGui::DragFloat3("position", reinterpret_cast<float *>(&directionalLight.pos), 0.01);        
        ImGui::DragFloat3("color", reinterpret_cast<float *>(&directionalLight.color), 0.01);        
        ImGui::DragFloat3("ambience", reinterpret_cast<float *>(&directionalLight.ambience), 0.01);
        ImGui::DragFloat("multiplier", &directionalLight.multiplier, 1);
        ImGui::DragFloat("orthoProj", &directionalLight.orthoProj, 1);
        ImGui::DragFloat("far", &directionalLight.far, 1);
        ImGui::DragFloat("near", &directionalLight.near, 1);
    }
};

struct ModelComponent {
public:
    ModelComponent(const std::string& file);
    ~ModelComponent();

private:
    std::string m_file;
};

} // namespace core

#endif
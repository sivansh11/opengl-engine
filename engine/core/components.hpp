#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "event.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <entt/entt.hpp>

#include <memory>
#include <vector>

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

    glm::vec3 m_pos{-1, 1, 0};
    glm::mat4 m_projection{1.0f};
    glm::mat4 m_view{1};
    float m_fov{45.0f};
    float m_far{1000.f}, m_near{0.1f};
};

struct PointLightComponent {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color{1, 1, 1};
    alignas(16) glm::vec3 term{.1, .1, .1};
};

struct SpotLightComponent {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color{1, 1, 1};
    alignas(16) glm::vec3 term{.1, .1, .1};
    alignas(16) glm::vec3 direction;
    float innerCutoff;
    float outerCutOff;
};

struct DirectionalLightComponent {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 ambience;
    glm::vec3 term;

    float multiplier = 1;
    float orthoProj = 10;
    float far = 7.5;
    float near = 1;
};

struct ChildrenComponent {
    std::vector<entt::entity> children;
};

} // namespace core

#endif
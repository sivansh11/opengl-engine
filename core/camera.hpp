#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "imgui.h"
#include "event_types.hpp"
#include "ecs.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace core {

class Camera {
public:
    void update(float aspect) {
        m_view = glm::lookAt(m_position, m_position + m_orientation, m_up);
        m_projection = glm::perspective(glm::radians(m_fov), aspect, m_nearPlane, m_farPlane);
    }

    glm::vec3& getPosition() { return m_position; }
    glm::vec3& getOrientation() { return m_orientation; }

    glm::mat4& getView() { return m_view; }
    glm::mat4& getProjection() { return m_projection; }

    static void componentPanel(Camera& camera, event::Dispatcher& dispatcher, ecs::EntityID ent) {
        ImGui::DragFloat3("Position", reinterpret_cast<float *>(&camera.m_position), 0.01);
        ImGui::DragFloat3("Orientation", reinterpret_cast<float *>(&camera.m_orientation), 0.01);
        ImGui::DragFloat("FOV", &camera.m_fov, 0.01f);
        ImGui::DragFloat("Far Plane", &camera.m_farPlane, 0.01f);
        ImGui::DragFloat("Near Plane", &camera.m_nearPlane, 0.01f);
        if (ImGui::Button("Set as active camera")) {
            dispatcher.post<SetActiveCameraEvent>(ent);
        }
    }

private:
    float m_fov = 60.f;
    float m_farPlane = 1000.0f, m_nearPlane = 0.01f;
    glm::vec3 m_position;
    glm::vec3 m_orientation{0, 0, 1};
    glm::vec3 m_up{0.0f, 1.0f, 0.0f};
    glm::mat4 m_view{1};
    glm::mat4 m_projection{1};

    uint32_t m_width, m_height;

};

} // namespace core

#endif
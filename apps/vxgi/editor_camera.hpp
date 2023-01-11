#ifndef EDITOR_CAMERA_HPP
#define EDITOR_CAMERA_HPP

#include "core/components.hpp"
#include "core/window.hpp"

class EditorCamera : public core::CameraComponent {
public:
    EditorCamera() {

    }
    ~EditorCamera() {

    }

    void onUpdate(float ts) {
        ImVec2 vp = ImGui::GetWindowSize();
        update(vp.x / vp.y);

        if (!ImGui::IsWindowHovered()) return;

        float velocity = m_speed * ts;

        if (ImGui::IsKeyDown(ImGuiKey_W)) 
            m_pos += m_front * velocity;
        if (ImGui::IsKeyDown(ImGuiKey_S)) 
            m_pos -= m_front * velocity;
        if (ImGui::IsKeyDown(ImGuiKey_D)) 
            m_pos += m_right * velocity;
        if (ImGui::IsKeyDown(ImGuiKey_A)) 
            m_pos -= m_right * velocity;
        if (ImGui::IsKeyDown(ImGuiKey_Space)) 
            m_pos += m_up * velocity;
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) 
            m_pos -= m_up * velocity;
        
        glm::vec2 mouse{ImGui::GetMousePos().x, ImGui::GetMousePos().y};
        glm::vec2 difference = mouse - m_initialMouse;
        m_initialMouse = mouse;

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
            difference.x = difference.x / vp.x;
            difference.y = -(difference.y / vp.y);

            m_yaw += difference.x * m_sensitivity;
            m_pitch += difference.y * m_sensitivity;

            if (m_pitch > 89.0f) 
                m_pitch = 89.0f;
            if (m_pitch < -89.0f) 
                m_pitch = -89.0f;
        }

        glm::vec3 front;
        front.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
        front.y = glm::sin(glm::radians(m_pitch));
        front.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
        m_front = front;
        m_right = glm::normalize(glm::cross(m_front, glm::vec3{0, 1, 0}));
        m_up    = glm::normalize(glm::cross(m_right, m_front));

        m_view = glm::lookAt(m_pos, m_pos + m_front, glm::vec3{0, 1, 0});
    } 


    float getYaw() { return m_yaw; }
    float getPitch() { return m_pitch; }
private:
    glm::vec3 m_front{0};
    glm::vec3 m_up{0, 1, 0};
    glm::vec3 m_right{0};

    glm::vec2 m_initialMouse{};

    float m_yaw = 0, m_pitch = 0;

    float m_speed = 3.f;
    float m_sensitivity = 100;

    bool is_first{true};
};

#endif
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "imgui.h"
#include "event_types.hpp"
#include "ecs.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace core {

class CameraComponent {
public:
    CameraComponent() {}
    ~CameraComponent() {}

    void update(float aspectRation) {
        m_projection = glm::perspective(glm::radians(m_fov), aspectRation, m_near, m_far);
    }

    glm::mat4& getProjection() {
        return m_projection;
    }

    static void componentPanel(CameraComponent& camera, event::Dispatcher& dispatcher, ecs::EntityID ent) {
        ImGui::DragFloat("FOV", &camera.m_fov, 0.01f);
        ImGui::DragFloat("Far Plane", &camera.m_far, 0.01f);
        ImGui::DragFloat("Near Plane", &camera.m_near, 0.01f);
    }

friend class EditorCamera;
private:
    glm::mat4 m_projection{1.0f};
    float m_fov{45.0f};
    float m_far{1000.f}, m_near{0.1f};
};

class EditorCamera : public CameraComponent {
public:
    EditorCamera() {

    }
    ~EditorCamera() {

    }

    void onUpdate(GLFWwindow *window, float ts) {
        ImVec2 vp = ImGui::GetWindowSize();
        update(vp.x / vp.y);

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
        
        // std::cout << ImGui::GetMousePos().x << ' ' << ImGui::GetMousePos().y << '\n';
        // std::cout << ImGui::GetWindowPos().x << ' ' << ImGui::GetWindowPos().y << '\n';
        // std::cout << ImGui::GetWindowSize().x << ' ' << ImGui::GetWindowSize().y << '\n';
        // std::cout << ImGui::GetWindowPos().x + (vp.x / 2) << ' ' << ImGui::GetWindowPos().y + (vp.y / 2) << '\n';
        // std::cout << "---------------------\n";

        glm::vec2 mouse{ImGui::GetMousePos().x, ImGui::GetMousePos().y};
        glm::vec2 difference = mouse - m_initialMouse;
        m_initialMouse = mouse;

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
            // ImGui::SetNextFrameWantCaptureMouse(true);
            // // ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            // ImVec2 winPos = ImGui::GetWindowPos();
            // ImVec2 mousePos = ImGui::GetMousePos();
            // if (is_first) {
                
            //     is_first = false;
            //     // glfwSetCursorPos(window, winPos.x + (vp.x / 2), winPos.y + (vp.y / 2));
            //     ImGui::SetCursorPos({winPos.x + (vp.x / 2), winPos.y + (vp.y / 2)});
            // }

            // glm::vec2 difference = glm::vec2{mousePos.x, mousePos.y} - glm::vec2{vp.x / 2, vp.y / 2};
            difference.x = difference.x / vp.x;
            difference.y = -(difference.y / vp.y);

            m_yaw += difference.x * m_sensitivity;
            m_pitch += difference.y * m_sensitivity;

            if (m_pitch > 89.0f) 
                m_pitch = 89.0f;
            if (m_pitch < -89.0f) 
                m_pitch = -89.0f;

            // glfwSetCursorPos(window, winPos.x + (vp.x / 2), winPos.y + (vp.y / 2));
            // ImGui::SetCursorPos({winPos.x + (vp.x / 2), winPos.y + (vp.y / 2)});

            
        }
        // else {
        //     is_first = true;
        //     ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
        //     ImGui::SetNextFrameWantCaptureMouse(false);
        // }

        glm::vec3 front;
        front.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
        front.y = glm::sin(glm::radians(m_pitch));
        front.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
        m_front = front;
        m_right = glm::normalize(glm::cross(m_front, glm::vec3{0, 1, 0}));
        m_up    = glm::normalize(glm::cross(m_right, m_front));

        m_view = glm::lookAt(m_pos, m_pos + m_front, glm::vec3{0, 1, 0});
    } 

    const glm::mat4& getView() { 
        return m_view; 
    }
    glm::vec3& getPos() { return m_pos; }

    static void componentPanel(EditorCamera& editorCamera) {
        ImGui::DragFloat3("Position", reinterpret_cast<float *>(&editorCamera.m_pos));
        ImGui::DragFloat("Sensitivity", &editorCamera.m_sensitivity, 0.01f);
        ImGui::DragFloat("Speed", &editorCamera.m_speed, 0.01f);
        ImGui::DragFloat("FOV", &editorCamera.m_fov, 0.01f);
        ImGui::DragFloat("Far Plane", &editorCamera.m_far, 0.01f);
        ImGui::DragFloat("Near Plane", &editorCamera.m_near, 0.01f);
    }
private:
    glm::vec3 m_pos{-1, 1, 0};
    glm::vec3 m_front{0};
    glm::vec3 m_up{0, 1, 0};
    glm::vec3 m_right{0};

    glm::mat4 m_view{1};
    glm::vec2 m_initialMouse{};

    float m_yaw = 0, m_pitch = 0;

    float m_speed = 3.f;
    float m_sensitivity = 100;

    bool is_first{true};
};

} // namespace core

#endif
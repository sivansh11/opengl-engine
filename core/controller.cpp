#include "controller.hpp"

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include <iostream>

#include "imgui.h"

namespace core {

void Controller::move(GLFWwindow* window, float dt, glm::vec3 &translation, glm::vec3 &rotation, glm::vec2 size, glm::vec2 offset)
{   

    glm::vec3 move{0};
    if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) move += rotation;
    if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) move -= rotation;
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) move += glm::normalize(glm::cross(rotation, up));
    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) move -= glm::normalize(glm::cross(rotation, up));
    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) move += up;
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) move -= up;

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
        ImGui::SetNextFrameWantCaptureMouse(true);
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);

        if (firstClick) {
            glfwSetCursorPos(window, (size.x / 2) + offset.x, (size.y / 2) + offset.y);
            // ImGui::SetCursorPos({size.x / 2, size.y / 2});
            firstClick = false;
        }

        double mx, my;
        glfwGetCursorPos(window, &mx, &my);

        mx -= offset.x;
        my -= offset.y;

        float rotX = lookSpeed * ((my - (size.y / 2)) / size.y);
        float rotY = lookSpeed * ((mx - (size.x / 2)) / size.x);

        glm::vec3 newRotation = glm::rotate(rotation, glm::radians(-rotX), glm::normalize(glm::cross(rotation, up)));
        
        if (glm::abs(glm::angle(newRotation, up) - glm::radians(90.f)) <= glm::radians(85.0f)) {
            rotation = newRotation;
        }

        rotation = glm::rotate(rotation, glm::radians(-rotY), up);

        glfwSetCursorPos(window, (size.x / 2) + offset.x, (size.y / 2) + offset.y);


    } else if (ImGui::IsMouseReleased(0)) {
        ImGui::SetNextFrameWantCaptureMouse(false);
        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
        firstClick = true;
    }


    if (glm::dot(move, move) > std::numeric_limits<float>::epsilon()) {
        translation += glm::normalize(move) * dt * moveSpeed;
    }
}   

void Controller::componentPanel(Controller& controller) {
    ImGui::DragFloat("Move Speed", &controller.moveSpeed, 0.01);
    ImGui::DragFloat("Look Speed", &controller.lookSpeed, 0.01);
}

} // namespace core

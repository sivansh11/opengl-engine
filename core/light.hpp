#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "event_types.hpp"
#include "ecs.hpp"

#include <glm/glm.hpp>
#include "imgui.h"

#include "gfx/shaders.hpp"

namespace core {

struct LightData {
    alignas(16) glm::vec3 pos;
    alignas(16) glm::vec3 color;
    alignas(16) glm::vec3 term;
    alignas(16) glm::vec3 ambience;
};

struct PointLight {
    glm::vec3 pos;
    // glm::vec3 ambient{0.1};
    glm::vec3 color{1};
    glm::vec3 term{.1, .1, .1};

    static void componentPanel(PointLight& pointLight, event::Dispatcher& dispatcher, ecs::EntityID ent) {
        ImGui::DragFloat3("Position", reinterpret_cast<float *>(&pointLight.pos), 0.01f);
        // ImGui::DragFloat3("Ambient", reinterpret_cast<float *>(&pointLight.ambient), 0.01f);
        // ImGui::DragFloat3("Diffuse", reinterpret_cast<float *>(&pointLight.diffuse), 0.01f);
        // ImGui::DragFloat3("Specular", reinterpret_cast<float *>(&pointLight.specular), 0.01f);
        // ImGui::ColorEdit3("Ambient", reinterpret_cast<float *>(&pointLight.ambient));
        ImGui::ColorEdit3("Diffuse", reinterpret_cast<float *>(&pointLight.color));
        // ImGui::DragFloat3("terms", reinterpret_cast<float *>(&pointLight.term), 0.01f);
        ImGui::DragFloat("C", &pointLight.term.x, 0.01f);
        ImGui::DragFloat("B", &pointLight.term.y, 0.01f);
        ImGui::DragFloat("A", &pointLight.term.z, 0.01f);
    }
};

struct AmbienceLight {
    glm::vec3 ambient{.01, .01, .01};
    static void componentPanel(AmbienceLight& gl, event::Dispatcher& dispatcher, ecs::EntityID ent) {
        ImGui::ColorEdit3("Ambience", reinterpret_cast<float *>(&gl.ambient));
    } 
};

} // namespace core

#endif
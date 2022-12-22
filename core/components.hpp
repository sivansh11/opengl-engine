#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "event_types.hpp"
#include "ecs.hpp"
#include "pyscript/script_engine.hpp"
#include "entity.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <unordered_map>
#include <string>


namespace core  {

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

struct TagComponent {
    std::string tag;

    static void componentPanel(TagComponent& tag) {
        ImGui::InputText("Tag", &tag.tag);
    }
};

struct Script {
    const char *name;
    py::object *object;
};

struct ScriptComponent {
    std::unordered_map<std::string, py::object> scripts;

    static void componentPanel(ScriptComponent& scriptComponent, event::Dispatcher& dispatcher, core::Scene32& scene, ecs::EntityID ent) {
        ImGui::Selectable("Drop Script Class", true);
        if (ImGui::BeginDragDropTarget()) {
            const ImGuiPayload *payload = ImGui::GetDragDropPayload();
            assert(payload != nullptr);
            if (std::memcmp(payload->DataType, "SCRIPT_CLASS", 13) != 0) {
                ImGui::EndDragDropTarget();
                return;
            }
            Script& script = *reinterpret_cast<Script *>(payload->Data);
            if (scriptComponent.scripts.contains(script.name)) {
                ImGui::EndDragDropTarget();
                return;
            }
            scriptComponent.scripts[script.name] = (*(script.object))(core::Entity(scene, ent));
            ImGui::EndDragDropTarget();
        }
        ImGui::Text("Active Scripts:");
        for (auto& kv : scriptComponent.scripts) {
            ImGui::Text("%s", kv.first.c_str());
        }
    }
};

struct LightData {
    alignas(16) glm::vec3 pos;
    alignas(16) glm::vec3 color;
    alignas(16) glm::vec3 term;
    alignas(16) glm::vec3 ambience;
};

struct PointLightComponent {
    // glm::vec3 ambient{0.1};
    glm::vec3 color{1};
    glm::vec3 term{.1, .1, .1};

    static void componentPanel(PointLightComponent& pointLight, event::Dispatcher& dispatcher, ecs::EntityID ent) {
        // ImGui::DragFloat3("Position", reinterpret_cast<float *>(&pointLight.pos), 0.01f);
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

struct AmbienceLightComponent {
    glm::vec3 ambient{.1, .1, .1};
    static void componentPanel(AmbienceLightComponent& gl, event::Dispatcher& dispatcher, ecs::EntityID ent) {
        ImGui::ColorEdit3("Ambience", reinterpret_cast<float *>(&gl.ambient));
    } 
};

// class CameraComponent {
// public:
//     CameraComponent() {}
//     ~CameraComponent() {}

//     void update(float aspectRation) {
//         m_projection = glm::perspective(glm::radians(m_fov), aspectRation, m_near, m_far);
//     }

//     glm::mat4& getProjection() {
//         return m_projection;
//     }

//     static void componentPanel(CameraComponent& camera, event::Dispatcher& dispatcher, ecs::EntityID ent) {
//         ImGui::DragFloat("FOV", &camera.m_fov, 0.01f);
//         ImGui::DragFloat("Far Plane", &camera.m_far, 0.01f);
//         ImGui::DragFloat("Near Plane", &camera.m_near, 0.01f);
//     }

// friend class EditorCamera;
// private:
//     glm::mat4 m_projection{1.0f};
//     float m_fov{45.0f};
//     float m_far{1000.f}, m_near{0.1f};
// };


} // namespace core

#endif
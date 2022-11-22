#ifndef IMGUI_PANELS_HPP
#define IMGUI_PANELS_HPP

#include "transform.hpp"
#include "tag.hpp"
#include "light.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "ecs.hpp"

#include <iostream>

struct Panel {
    Panel(const char *name) {
        ImGui::Begin(name);
    }
    ~Panel() {
        ImGui::End();
    }
};

void drawEntityNode(ecs::Scene& scene, event::Dispatcher& dispatcher, ecs::EntityID ent, ecs::EntityID& selectedEnt) {
    auto& tag = scene.get<core::Tag>(ent).tag;
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
    bool opened = ImGui::TreeNodeEx(tag.c_str(), flags);

    if (ImGui::BeginPopupContextItem(std::to_string(ent).c_str())) {
        if (!scene.has<core::Transform>(ent) && ImGui::MenuItem("Add Transform")) {
            scene.assign<core::Transform>(ent);
        }
        if (!scene.has<core::PointLight>(ent) && ImGui::MenuItem("Add PointLight")) {
            scene.assign<core::PointLight>(ent);
        }
        if (!scene.has<core::Model>(ent) && ImGui::MenuItem("Add model")) {
            scene.assign<core::Model>(ent);
        }
        if (scene.has<core::PointLight>(ent) && !scene.has<core::AmbienceLight>(ent) &&  ImGui::MenuItem("Add Ambience Light")) {
            scene.assign<core::AmbienceLight>(ent);
        }
        if (scene.has<core::Transform>(ent) && ImGui::MenuItem("Control")) {
            dispatcher.post<core::SetActiveKeyboardControllerEntityEvent>(ent);
        }
        
        if (ImGui::MenuItem("Delete Entity")) {
            scene.deleteEntity(ent);
            if (ent == selectedEnt) {
                selectedEnt = ecs::null;
            }
        }

        ImGui::EndPopup();
    }

    if (ImGui::IsItemClicked() && ImGui::IsMouseDown(0)) {
        selectedEnt = ent;
    }

    if (opened) {
        ImGui::TreePop();
    }
}

void frameTime() {
    Panel p("FrameTime");
    ImGui::Text("%.5f ms  %f fps", ImGui::GetIO().DeltaTime * 1000, ImGui::GetIO().Framerate);
}

void sceneHirarchy(ecs::Scene *scene, event::Dispatcher& dispatcher, ecs::EntityID& selectedEnt) {
    Panel p{"ScreenHierarchy"};
    if (!scene) return;
    
    for (auto& ent : ecs::SceneView<>(*scene)) {
        drawEntityNode(*scene, dispatcher, ent, selectedEnt);
    }

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
        selectedEnt = ecs::null;
    }

    ImGuiPopupFlags flags = ImGuiPopupFlags_NoOpenOverItems |
                            ImGuiPopupFlags_MouseButtonRight;
    if (ImGui::BeginPopupContextWindow(NULL, flags)) {
        if (ImGui::MenuItem("Create Empty Entity")) {
            auto newEnt = scene->newEntity();
            scene->assign<core::Tag>(newEnt).tag = "New Entity";
        }
        ImGui::EndPopup();
    }
}

void selectedEntityComponentViewer(ecs::Scene *scene, event::Dispatcher& dispatcher, ecs::EntityID ent) {
    Panel p{"Selected Entity Components Viewer"};
    if (!scene) return;
    if (ent == ecs::null) return;

        ImGui::Text("Tag Component");
    core::Tag::componentPanel(scene->get<core::Tag>(ent));
    ImGui::Separator();

    if (scene->has<core::Transform>(ent)) {
        ImGui::Text("Transform Component");
        core::Transform::componentPanel(scene->get<core::Transform>(ent), dispatcher, ent);
        ImGui::Separator();
    }
    if (scene->has<core::PointLight>(ent)) {
        ImGui::Text("Point Light Component");
        core::PointLight::componentPanel(scene->get<core::PointLight>(ent), dispatcher, ent);
        ImGui::Separator();
    }
    if (scene->has<core::AmbienceLight>(ent)) {
        ImGui::Text("Ambience Light Component");
        core::AmbienceLight::componentPanel(scene->get<core::AmbienceLight>(ent), dispatcher, ent);
        ImGui::Separator();
    }
    if (scene->has<core::Camera>(ent)) {
        ImGui::Text("Camera Component");
        core::Camera::componentPanel(scene->get<core::Camera>(ent), dispatcher, ent);
        ImGui::Separator();
    }
    if (scene->has<core::Model>(ent)) {
        ImGui::Text("Model Component");
        core::Model::componentPanel(scene->get<core::Model>(ent), dispatcher, ent);
        ImGui::Separator();
    }
}

#endif
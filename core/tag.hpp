#ifndef TAG_HPP
#define TAG_HPP

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include <string>

namespace core {

struct TagComponent {
    std::string tag;

    static void componentPanel(TagComponent& tag) {
        ImGui::InputText("Tag", &tag.tag);
    }
};

} // namespace core

#endif
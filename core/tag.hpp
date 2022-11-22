#ifndef TAG_HPP
#define TAG_HPP

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include <string>

namespace core {

struct Tag {
    std::string tag;

    static void componentPanel(Tag& tag) {
        ImGui::InputText("Tag", &tag.tag);
    }
};

} // namespace core

#endif
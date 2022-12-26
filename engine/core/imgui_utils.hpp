#ifndef IMGUI_UTILS_HPP
#define IMGUI_UTILS_HPP

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace core {

void initImgui(GLFWwindow *window);

void terminateImgui();

void startFrameImgui();

void endFrameImgui(GLFWwindow *window);

struct ImVec3 { float x, y, z; ImVec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) { x = _x; y = _y; z = _z; } };

void easyThemingImGui(ImVec3 color_for_text, ImVec3 color_for_head, ImVec3 color_for_area, ImVec3 color_for_body, ImVec3 color_for_pops, ImVec3 color_for_tab);

void setupImGuiStyle();

void myDefaultImGuiStyle();

} // namespace core

#endif
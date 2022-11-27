#include "script_engine.hpp"

#include "../entity.hpp"
#include "imgui.h"

#include "../transform.hpp"
#include "../light.hpp"

#include <iostream>
#include <sstream>

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(engine_native, m) {
    m.def("register_script", &core::pyscript::ScriptEngine::registerScript);
    // TODO: mouse
    m.def("is_key_down", [](const ImGuiKey key) {
        return ImGui::IsKeyDown(key);
    });
    m.def("is_key_pressed", [](const ImGuiKey key) {
        return ImGui::IsKeyPressed(key);
    });
    m.def("is_key_released", [](const ImGuiKey key) {
        return ImGui::IsKeyReleased(key);
    });

    // TODO: add other imgui stuff
    m.def("begin", [](std::string& title) {
        return ImGui::Begin(title.c_str());
    });
    m.def("end", []() {
        ImGui::End();
    });
    m.def("drag_float", [](std::string& label, glm::vec1& v, float v_speed, float v_min, float v_max) {
        ImGui::DragFloat(label.c_str(), &v.x, v_speed, v_min, v_max);
    });
}

PYBIND11_EMBEDDED_MODULE(engine_type, m) {
    py::enum_<ImGuiKey>(m, "Key")
        .value("LeftArrow", ImGuiKey_LeftArrow)
        .value("RightArrow", ImGuiKey_RightArrow)
        .value("UpArrow", ImGuiKey_UpArrow)
        .value("DownArrow", ImGuiKey_DownArrow)
        .value("PageUp", ImGuiKey_PageUp)
        .value("PageDown", ImGuiKey_PageDown)
        .value("Home", ImGuiKey_Home)
        .value("End", ImGuiKey_End)
        .value("Insert", ImGuiKey_Insert)
        .value("Delete", ImGuiKey_Delete)
        .value("Backspace", ImGuiKey_Backspace)
        .value("Space", ImGuiKey_Space)
        .value("Enter", ImGuiKey_Enter)
        .value("Escape", ImGuiKey_Escape)
        .value("LeftCtrl", ImGuiKey_LeftCtrl) 
        .value("LeftShift", ImGuiKey_LeftShift) 
        .value("LeftAlt", ImGuiKey_LeftAlt) 
        .value("LeftSuper", ImGuiKey_LeftSuper)
        .value("RightCtrl", ImGuiKey_RightCtrl) 
        .value("RightShift", ImGuiKey_RightShift) 
        .value("RightAlt", ImGuiKey_RightAlt) 
        .value("RightSuper", ImGuiKey_RightSuper)
        .value("Menu", ImGuiKey_Menu)
        .value("0", ImGuiKey_0) 
        .value("1", ImGuiKey_1) 
        .value("2", ImGuiKey_2) 
        .value("3", ImGuiKey_3) 
        .value("4", ImGuiKey_4) 
        .value("5", ImGuiKey_5) 
        .value("6", ImGuiKey_6) 
        .value("7", ImGuiKey_7) 
        .value("8", ImGuiKey_8) 
        .value("9", ImGuiKey_9)
        .value("A", ImGuiKey_A) 
        .value("B", ImGuiKey_B) 
        .value("C", ImGuiKey_C) 
        .value("D", ImGuiKey_D) 
        .value("E", ImGuiKey_E) 
        .value("F", ImGuiKey_F) 
        .value("G", ImGuiKey_G) 
        .value("H", ImGuiKey_H) 
        .value("I", ImGuiKey_I) 
        .value("J", ImGuiKey_J)
        .value("K", ImGuiKey_K) 
        .value("L", ImGuiKey_L) 
        .value("M", ImGuiKey_M) 
        .value("N", ImGuiKey_N) 
        .value("O", ImGuiKey_O) 
        .value("P", ImGuiKey_P) 
        .value("Q", ImGuiKey_Q) 
        .value("R", ImGuiKey_R) 
        .value("S", ImGuiKey_S) 
        .value("T", ImGuiKey_T)
        .value("U", ImGuiKey_U) 
        .value("V", ImGuiKey_V) 
        .value("W", ImGuiKey_W) 
        .value("X", ImGuiKey_X) 
        .value("Y", ImGuiKey_Y) 
        .value("Z", ImGuiKey_Z)
        .value("F1", ImGuiKey_F1) 
        .value("F2", ImGuiKey_F2) 
        .value("F3", ImGuiKey_F3) 
        .value("F4", ImGuiKey_F4) 
        .value("F5", ImGuiKey_F5) 
        .value("F6", ImGuiKey_F6)
        .value("F7", ImGuiKey_F7) 
        .value("F8", ImGuiKey_F8) 
        .value("F9", ImGuiKey_F9) 
        .value("F10", ImGuiKey_F10) 
        .value("F11", ImGuiKey_F11) 
        .value("F12", ImGuiKey_F12)
        .value("Apostrophe", ImGuiKey_Apostrophe)        // '
        .value("Comma", ImGuiKey_Comma)             // ,
        .value("Minus", ImGuiKey_Minus)             // -
        .value("Period", ImGuiKey_Period)            // .
        .value("Slash", ImGuiKey_Slash)             // /
        .value("Semicolon", ImGuiKey_Semicolon)         // ;
        .value("Equal", ImGuiKey_Equal)             // =
        .value("LeftBracket", ImGuiKey_LeftBracket)       // [
        .value("Backslash", ImGuiKey_Backslash)         // \ (this text inhibit multiline comment caused by backslash)
        .value("RightBracket", ImGuiKey_RightBracket)      // ]
        .value("GraveAccent", ImGuiKey_GraveAccent)       // `
        .value("CapsLock", ImGuiKey_CapsLock)
        .value("ScrollLock", ImGuiKey_ScrollLock)
        .value("NumLock", ImGuiKey_NumLock)
        .value("PrintScreen", ImGuiKey_PrintScreen)
        .value("Pause", ImGuiKey_Pause)
        .value("Keypad0", ImGuiKey_Keypad0) 
        .value("Keypad1", ImGuiKey_Keypad1) 
        .value("Keypad2", ImGuiKey_Keypad2) 
        .value("Keypad3", ImGuiKey_Keypad3) 
        .value("Keypad4", ImGuiKey_Keypad4)
        .value("Keypad5", ImGuiKey_Keypad5) 
        .value("Keypad6", ImGuiKey_Keypad6) 
        .value("Keypad7", ImGuiKey_Keypad7) 
        .value("Keypad8", ImGuiKey_Keypad8) 
        .value("Keypad9", ImGuiKey_Keypad9)
        .value("KeypadDecimal", ImGuiKey_KeypadDecimal)
        .value("KeypadDivide", ImGuiKey_KeypadDivide)
        .value("KeypadMultiply", ImGuiKey_KeypadMultiply)
        .value("KeypadSubtract", ImGuiKey_KeypadSubtract)
        .value("KeypadAdd", ImGuiKey_KeypadAdd)
        .value("KeypadEnter", ImGuiKey_KeypadEnter)
        .value("KeypadEqual", ImGuiKey_KeypadEqual)
        ;
    py::class_<core::TransformComponent>(m, "Transform")
        .def_readwrite("translation", &core::TransformComponent::translation)
        .def_readwrite("rotation", &core::TransformComponent::rotation)
        .def_readwrite("scale", &core::TransformComponent::scale)
        ;

    py::class_<core::PointLightComponent>(m, "PointLight")
        .def_readwrite("color", &core::PointLightComponent::color)
        .def_readwrite("term", &core::PointLightComponent::term)
        ;

    py::class_<core::AmbienceLightComponent>(m, "AmbienceLight")
        .def_readwrite("ambient", &core::AmbienceLightComponent::ambient)
        ;

    py::class_<core::Entity>(m, "Entity")
        .def(py::init<const core::Entity&>())

        .def("has_transform", [](core::Entity& e) {
            return e.has<core::TransformComponent>();
        })
        .def("get_transform", [](core::Entity& e) -> core::TransformComponent& {
            return e.get<core::TransformComponent>();
        }, py::return_value_policy::reference)
        .def("assign_transform", [](core::Entity& e) {
            e.assign<core::TransformComponent>();
        })

        .def("has_point_light", [](core::Entity& e) {
            return e.has<core::PointLightComponent>();
        })
        .def("get_point_light", [](core::Entity& e) -> core::PointLightComponent& {
            return e.get<core::PointLightComponent>();
        }, py::return_value_policy::reference)
        .def("assign_point_light", [](core::Entity& e) {
            e.assign<core::PointLightComponent>();
        })

        .def("has_ambience_light", [](core::Entity& e) {
            return e.has<core::AmbienceLightComponent>();
        })
        .def("get_ambience_light", [](core::Entity& e) -> core::AmbienceLightComponent& {
            return e.get<core::AmbienceLightComponent>();
        }, py::return_value_policy::reference)
        .def("assign_ambience_light", [](core::Entity& e) {
            e.assign<core::AmbienceLightComponent>();
        })
        ;
}

PYBIND11_EMBEDDED_MODULE(engine_math, m) {
    py::class_<glm::vec1>(m, "vec1")
        .def(py::init<float>())
        .def(py::init<glm::vec1&>())
        .def(py::init())
        .def("__repr__", [](const glm::vec1& vec){
            std::stringstream out;
            out << '(' << vec.x << ')';
            return out.str();
        })
        .def(py::self + py::self)
        .def(py::self += py::self)
        .def(py::self + float())
        .def(py::self += float())
        .def(py::self - py::self)
        .def(py::self -= py::self)
        .def(py::self - float())
        .def(py::self -= float())
        .def(py::self * float())
        .def(py::self *= float())
        .def(py::self / float())
        .def(py::self /= float())
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def_readwrite("x", &glm::vec1::x)
        .def_readwrite("r", &glm::vec1::x)
        ;
    py::class_<glm::vec2>(m, "vec2")
        .def(py::init<float, float>())
        .def(py::init<float>())
        .def(py::init())
        .def(py::init<glm::vec2&>())
        .def("__repr__", [](const glm::vec2& vec){
            std::stringstream out;
            out << '(' << vec.x << ' ' << vec.y << ')';
            return out.str();
        })
        .def(py::self + py::self)
        .def(py::self += py::self)
        .def(py::self + float())
        .def(py::self += float())
        .def(py::self - py::self)
        .def(py::self -= py::self)
        .def(py::self - float())
        .def(py::self -= float())
        .def(py::self * float())
        .def(py::self *= float())
        .def(py::self / float())
        .def(py::self /= float())
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def_readwrite("x", &glm::vec2::x)
        .def_readwrite("y", &glm::vec2::y)
        .def_readwrite("r", &glm::vec2::x)
        .def_readwrite("b", &glm::vec2::y)
        ;

    py::class_<glm::vec3>(m, "vec3")
        .def(py::init<float, float, float>())
        .def(py::init<float>())
        .def(py::init())
        .def(py::init<glm::vec3&>())
        .def("__repr__", [](const glm::vec3& vec){
            std::stringstream out;
            out << '(' << vec.x << ' ' << vec.y << ' ' << vec.z << ')';
            return out.str();
        })
        .def(py::self + py::self)
        .def(py::self += py::self)
        .def(py::self + float())
        .def(py::self += float())
        .def(py::self - py::self)
        .def(py::self -= py::self)
        .def(py::self - float())
        .def(py::self -= float())
        .def(py::self * float())
        .def(py::self *= float())
        .def(py::self / float())
        .def(py::self /= float())
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def_readwrite("x", &glm::vec3::x)
        .def_readwrite("y", &glm::vec3::y)
        .def_readwrite("z", &glm::vec3::z)
        .def_readwrite("r", &glm::vec3::x)
        .def_readwrite("g", &glm::vec3::y)
        .def_readwrite("b", &glm::vec3::z);

    py::class_<glm::vec4>(m, "vec4")
        .def(py::init<float, float, float, float>())
        .def(py::init<float>())
        .def(py::init())
        .def(py::init<glm::vec4&>())
        .def("__repr__", [](const glm::vec4& vec){
            std::stringstream out;
            out << '(' << vec.x << ' ' << vec.y << ' ' << vec.z << ' ' << vec.w << ')';
            return out.str();
        })
        .def(py::self + py::self)
        .def(py::self += py::self)
        .def(py::self + float())
        .def(py::self += float())
        .def(py::self - py::self)
        .def(py::self -= py::self)
        .def(py::self - float())
        .def(py::self -= float())
        .def(py::self * float())
        .def(py::self *= float())
        .def(py::self / float())
        .def(py::self /= float())
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def_readwrite("x", &glm::vec4::x)
        .def_readwrite("y", &glm::vec4::y)
        .def_readwrite("z", &glm::vec4::z)
        .def_readwrite("w", &glm::vec4::w)
        .def_readwrite("r", &glm::vec4::x)
        .def_readwrite("g", &glm::vec4::y)
        .def_readwrite("b", &glm::vec4::z)
        .def_readwrite("a", &glm::vec4::w);
    
}

namespace core {

namespace pyscript {

struct __attribute__((visibility("hidden"))) PyData {
    py::scoped_interpreter interpreter{};
    py::object scope{};
    std::unordered_map<std::string, py::object> nameToScriptMap;
};

static PyData *p_pyData;

bool ScriptEngine::init() {
    p_pyData = new PyData{};
    p_pyData->scope = py::module_::import("__main__").attr("__dict__");
    return p_pyData ? true : false;
}

void ScriptEngine::free() {
    delete p_pyData;
}

void ScriptEngine::exec_script(std::string filePath) {
    py::eval_file(filePath.c_str(), p_pyData->scope);
}

void ScriptEngine::registerScript(std::string& name, py::object script) {
    p_pyData->nameToScriptMap[name] = script;
}

std::unordered_map<std::string, py::object>& ScriptEngine::getObjects() {
    return p_pyData->nameToScriptMap;
}

} // namespace pyscript

} // namespace core

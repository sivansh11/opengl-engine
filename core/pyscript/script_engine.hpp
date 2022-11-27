#ifndef SCRIPT_ENGINE_HPP
#define SCRIPT_ENGINE_HPP

#include <pybind11/embed.h>
#include <pybind11/operators.h>

#include <string>

#include <unordered_map>

namespace py = pybind11;

namespace core {
    
namespace pyscript {

class ScriptEngine {
public:
    static bool init();
    static void free();

    static void exec_script(std::string scriptPath);

    static void registerScript(std::string& name, py::object script);

    static std::unordered_map<std::string, py::object>& getObjects();
};

} // namespace pyscript

} // namespace core

#endif
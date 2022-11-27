#include "script_manager.hpp"

namespace core {

ScriptManager::ScriptManager() {
    pyscript::ScriptEngine::init();
}

ScriptManager::~ScriptManager() {
    pyscript::ScriptEngine::free();
}

void ScriptManager::runScript(const std::string& scriptPath) {
    m_filePaths.push_back(scriptPath);
    pyscript::ScriptEngine::exec_script(scriptPath.c_str());
}

void ScriptManager::reload() {
    for (auto& scriptPath : m_filePaths) {
        pyscript::ScriptEngine::exec_script(scriptPath.c_str());
    }
}

} // namespace core

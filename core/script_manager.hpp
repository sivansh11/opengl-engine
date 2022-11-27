#ifndef SCRIPT_MANAGER_HPP
#define SCRIPT_MANAGER_HPP

#include "pyscript/script_engine.hpp"

#include <vector>
#include <string>

namespace core {

class ScriptManager {
public:
    ScriptManager();
    ~ScriptManager();

    void runScript(const std::string& scriptPath);

    void reload();

private:
    std::vector<std::string> m_filePaths;
};

} // namespace core

#endif
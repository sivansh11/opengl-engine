#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <string>

namespace core {

class AssetManager {
public:
    AssetManager() = default;
    AssetManager(const std::string& path);
    ~AssetManager();



private:
    std::string m_path;
};

} // namespace core

#endif
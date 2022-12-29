#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include "entity.hpp"

#include "yaml-cpp/yaml.h"

#include <memory>

namespace core {

class Serializer {
public:
    Serializer() = default;
    ~Serializer() = default;

    static void saveScene(const std::shared_ptr<core::Scene> scene, const std::string& filePath);
    static std::shared_ptr<core::Scene> loadScene(const std::string& filePath);
private:

};

} // namespace core

#endif
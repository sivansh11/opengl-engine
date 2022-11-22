#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "gfx/texture.hpp"
#include "gfx/shaders.hpp"

#include "utils.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <map>
#include <set>
#include <any>

namespace core {

enum MaterialUniformType {
    SAMPLER_2D,
    FLOAT,
    VEC2,
    VEC3,
    VEC4,
    BOOL,
};

struct Material {
public:
    Material() = default;
    Material(Material&& material);
    Material& operator=(Material&& material);

    Material(Material&) = delete;
    Material& operator=(Material&) = delete;

    struct MaterialTypeBuilder {
        std::map<std::string, MaterialUniformType> nameToUniformType;
        MaterialTypeBuilder& addSampler2D(const char *name);
        MaterialTypeBuilder& addFloat(const char *name);
        MaterialTypeBuilder& addVec2(const char *name);
        MaterialTypeBuilder& addVec3(const char *name);
        MaterialTypeBuilder& addVec4(const char *name);
        MaterialTypeBuilder& addBool(const char *name);
        Material* build();
    };

    void bind(gfx::ShaderProgram& shader);
    void assign(const char *name, std::any data);

    bool operator==(const Material& other) const {
        if (uniformNames.size() != other.uniformNames.size()) {
            return false;
        }
        for (const auto& name : uniformNames) {
            if (!other.uniformNames.contains(name)) {
                return false;
            }
            if (nameToUniformType.at(name) != other.nameToUniformType.at(name)) {
                return false;
            }
        }
        return true;
    }

private:
    std::set<std::string> uniformNames;
    std::map<std::string, MaterialUniformType> nameToUniformType;
    std::map<std::string, std::shared_ptr<gfx::Texture2D>> nameToUniformSampler2D;
    std::map<std::string, float> nameToUniformFloat;
    std::map<std::string, glm::vec2> nameToUniformVec2;
    std::map<std::string, glm::vec3> nameToUniformVec3;
    std::map<std::string, glm::vec4> nameToUniformVec4;
    std::map<std::string, bool> nameToUniformBool;

    friend class std::hash<Material>;
};

} // namespace core

namespace std {

template <>
struct hash<core::Material> {
    size_t operator()(core::Material const& material) const {
        size_t seed = 0;
        for (const auto& name : material.uniformNames) {
            core::hashCombine(seed, name, material.nameToUniformType.at(name));
        }
        return seed;
    }
};

} // namespace std


#endif
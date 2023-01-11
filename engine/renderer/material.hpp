#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "../gfx/texture.hpp"
#include "../gfx/shaders.hpp"

#include "../core/utils.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <map>
#include <set>
#include <any>

namespace renderer {

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
        MaterialTypeBuilder& addSampler2D(const std::string& name);
        MaterialTypeBuilder& addFloat(const std::string& name);
        MaterialTypeBuilder& addVec2(const std::string& name);
        MaterialTypeBuilder& addVec3(const std::string& name);
        MaterialTypeBuilder& addVec4(const std::string& name);
        MaterialTypeBuilder& addBool(const std::string& name);
        Material* build();
    };

    void bind(gfx::ShaderProgram& shader);
    void assign(const std::string& name, std::any data);

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
    std::map<std::string, std::shared_ptr<gfx::Texture>> nameToUniformSampler2D;
    std::map<std::string, float> nameToUniformFloat;
    std::map<std::string, glm::vec2> nameToUniformVec2;
    std::map<std::string, glm::vec3> nameToUniformVec3;
    std::map<std::string, glm::vec4> nameToUniformVec4;
    std::map<std::string, bool> nameToUniformBool;

    friend class std::hash<Material>;
};

} // namespace renderer

namespace std {

template <>
struct hash<renderer::Material> {
    size_t operator()(renderer::Material const& material) const {
        size_t seed = 0;
        for (const auto& name : material.uniformNames) {
            core::hashCombine(seed, name, material.nameToUniformType.at(name));
        }
        return seed;
    }
};

} // namespace std
#endif
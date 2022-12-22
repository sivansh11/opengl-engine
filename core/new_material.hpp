#ifndef NEW_MATERIAL_HPP
#define NEW_MATERIAL_HPP

#include "gfx/texture.hpp"
#include "gfx/shaders.hpp"

#include <memory>

namespace core {

struct NewMaterial {
    std::shared_ptr<gfx::Texture2D> diffuseMap;
    std::shared_ptr<gfx::Texture2D> specularMap;
    std::shared_ptr<gfx::Texture2D> normalMap;
    bool hasDiffuseMap;
    bool hasSpecularMap;
    bool hasNormalMap;

    void bind(gfx::ShaderProgram& shader) {
        if (hasDiffuseMap) {
            shader.veci("hasDiffuseMap", true);
            diffuseMap->bind("diffuseMap", 0, shader);
        } else {
            shader.veci("hasDiffuseMap", false);
        }
        if (hasSpecularMap) {
            shader.veci("hasSpecularMap", true);
            specularMap->bind("specularMap", 0, shader);
        } else {
            shader.veci("hasSpecularMap", false);
        }
        if (hasNormalMap) {
            shader.veci("hasNormalMap", true);
            normalMap->bind("normalMap", 0, shader);
        } else {
            shader.veci("hasNormalMap", false);
        }
    }
};

} // namespace core

#endif
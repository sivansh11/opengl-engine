#include "material.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cstring>

namespace renderer {
    
Material::MaterialTypeBuilder& Material::MaterialTypeBuilder::addSampler2D(const char *name) {
    nameToUniformType[name] = MaterialUniformType::SAMPLER_2D;
    return *this;
}

Material::MaterialTypeBuilder& Material::MaterialTypeBuilder::addFloat(const char *name) {
    nameToUniformType[name] = MaterialUniformType::FLOAT;
    return *this;
}

Material::MaterialTypeBuilder& Material::MaterialTypeBuilder::addVec2(const char *name) {
    nameToUniformType[name] = MaterialUniformType::VEC2;
    return *this;
}

Material::MaterialTypeBuilder& Material::MaterialTypeBuilder::addVec3(const char *name) {
    nameToUniformType[name] = MaterialUniformType::VEC3;
    return *this;
}

Material::MaterialTypeBuilder& Material::MaterialTypeBuilder::addVec4(const char *name) {
    nameToUniformType[name] = MaterialUniformType::VEC4;
    return *this;
}

Material::MaterialTypeBuilder& Material::MaterialTypeBuilder::addBool(const char *name) {
    nameToUniformType[name] = MaterialUniformType::BOOL;
    return *this;
}

Material* Material::MaterialTypeBuilder::build() {
    Material *material = new Material;
    material->nameToUniformType = nameToUniformType;
    for (auto& kv : nameToUniformType) {
        auto& name = kv.first;
        auto& type = kv.second;

        material->uniformNames.emplace(name);

        switch (type) {
            case MaterialUniformType::SAMPLER_2D:
                material->nameToUniformSampler2D[name];
                break;
            case MaterialUniformType::FLOAT:
                material->nameToUniformFloat[name];
                break;
            case MaterialUniformType::VEC2:
                material->nameToUniformVec2[name];
                break;
            case MaterialUniformType::VEC3:
                material->nameToUniformVec3[name];
                break;
            case MaterialUniformType::VEC4:
                material->nameToUniformVec4[name];
                break;
            case MaterialUniformType::BOOL:
                material->nameToUniformBool[name];
                break;
            
            default:
                break;
        }
    }

    return material;
}

Material::Material(Material&& material) {
    uniformNames = std::move(material.uniformNames);
    nameToUniformType = std::move(material.nameToUniformType);
    nameToUniformSampler2D = std::move(material.nameToUniformSampler2D);
    nameToUniformFloat = std::move(material.nameToUniformFloat);
    nameToUniformVec2 = std::move(material.nameToUniformVec2);
    nameToUniformVec3 = std::move(material.nameToUniformVec3);
    nameToUniformVec4 = std::move(material.nameToUniformVec4);
    nameToUniformBool = std::move(material.nameToUniformBool);
}

Material& Material::operator=(Material&& material) {
    uniformNames = std::move(material.uniformNames);
    nameToUniformType = std::move(material.nameToUniformType);
    nameToUniformSampler2D = std::move(material.nameToUniformSampler2D);
    nameToUniformFloat = std::move(material.nameToUniformFloat);
    nameToUniformVec2 = std::move(material.nameToUniformVec2);
    nameToUniformVec3 = std::move(material.nameToUniformVec3);
    nameToUniformVec4 = std::move(material.nameToUniformVec4);
    nameToUniformBool = std::move(material.nameToUniformBool);

    return *this;
}


void Material::assign(const char *name, std::any data) {
    assert(nameToUniformType.contains(name));
    auto& type = nameToUniformType[name];
    switch (type) {
        case MaterialUniformType::SAMPLER_2D:
            nameToUniformSampler2D[name] = std::any_cast<std::shared_ptr<gfx::Texture2D>>(data);
            break;
        case MaterialUniformType::FLOAT:
            nameToUniformFloat[name] = std::any_cast<float>(data);
            break;
        case MaterialUniformType::VEC2:
            nameToUniformVec2[name] = std::any_cast<glm::vec2>(data);
            break;
        case MaterialUniformType::VEC3:
            nameToUniformVec3[name] = std::any_cast<glm::vec3>(data);
            break;
        case MaterialUniformType::VEC4:
            nameToUniformVec4[name] = std::any_cast<glm::vec4>(data);
            break;
        case MaterialUniformType::BOOL:
            nameToUniformBool[name] = std::any_cast<bool>(data);
            break;
        
        default:
            break;
    }
}

void Material::bind(gfx::ShaderProgram& shader) {
    uint32_t i = 0;
    for (auto& name : uniformNames) {
        auto& type = nameToUniformType[name];
        switch (type) {
            case MaterialUniformType::SAMPLER_2D:
                nameToUniformSampler2D[name]->bind(name.c_str(), i++, shader);
                break;
            case MaterialUniformType::FLOAT:
                shader.vecf(name.c_str(), nameToUniformFloat[name]);
                break;
            case MaterialUniformType::VEC2:
                shader.vec2f(name.c_str(), glm::value_ptr(nameToUniformVec2[name]));
                break;
            case MaterialUniformType::VEC3:
                shader.vec3f(name.c_str(), glm::value_ptr(nameToUniformVec3[name]));
                break;
            case MaterialUniformType::VEC4:
                shader.vec4f(name.c_str(), glm::value_ptr(nameToUniformVec4[name]));
                break;
            case MaterialUniformType::BOOL:
                shader.veci(name.c_str(), nameToUniformBool[name]);
                break;

            default:
                break;
        }
    }
}

} // namespace renderer

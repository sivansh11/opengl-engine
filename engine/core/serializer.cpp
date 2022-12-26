#include "serializer.hpp"

#include "components.hpp"

#include <glm/glm.hpp>

#include <fstream>
#include <iostream>

namespace YAML {

template <>
struct convert<glm::vec1> {
    static Node encode(const glm::vec1& rhs) {
        Node node;
        node.push_back(rhs.x);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }
    static bool decode(const Node& node, glm::vec1& rhs) {
        if (!node.IsSequence() || node.size() != 1) {
            return false;
        }
        rhs.x = node[0].as<float>();
        return true;
    }
};

template <>
struct convert<glm::vec2> {
    static Node encode(const glm::vec2& rhs) {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }
    static bool decode(const Node& node, glm::vec2& rhs) {
        if (!node.IsSequence() || node.size() != 2) {
            return false;
        }
        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        return true;
    }
};

template <>
struct convert<glm::vec3> {
    static Node encode(const glm::vec3& rhs) {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }
    static bool decode(const Node& node, glm::vec3& rhs) {
        if (!node.IsSequence() || node.size() != 3) {
            return false;
        }
        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        return true;
    }
};

template <>
struct convert<glm::vec4> {
    static Node encode(const glm::vec4& rhs) {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.push_back(rhs.w);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }
    static bool decode(const Node& node, glm::vec4& rhs) {
        if (!node.IsSequence() || node.size() != 4) {
            return false;
        }
        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        rhs.w = node[3].as<float>();
        return true;
    }
};

} // namespace YAML

#define DEFINE_WRITE_OPERATOR(T) \
YAML::Emitter& operator << (YAML::Emitter& out, const T& val) { \
    out << YAML::Node(val); \
    return out; \
}

namespace core {

DEFINE_WRITE_OPERATOR(glm::vec1)
DEFINE_WRITE_OPERATOR(glm::vec2)
DEFINE_WRITE_OPERATOR(glm::vec3)
DEFINE_WRITE_OPERATOR(glm::vec4)

static void serializeEntity(YAML::Emitter& out, const std::unique_ptr<core::Scene>& scene, ecs::EntityID ent) {
    if (scene->has<TagComponent>(ent)) {
        auto& tag = scene->get<TagComponent>(ent).tag;
        out << YAML::Key << "Tag";
        out << YAML::Value << tag;
    } else {
        assert(false);
    }
        
    if (scene->has<TransformComponent>(ent)) {
        auto tc = scene->get<TransformComponent>(ent);
        out << YAML::Key << "TransformComponent";
        out << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "translation";
            out << YAML::Value << tc.translation;
            out << YAML::Key << "rotation";
            out << YAML::Value << tc.rotation;
            out << YAML::Key << "scale";
            out << YAML::Value << tc.scale;
        out << YAML::EndMap;
    }

    // if (scene->has<RenderableComponent>(ent)) {
    //     auto& rc = scene->get<RenderableComponent>(ent);
    //     out << YAML::Key << "RenderableComponent";
    //     out << YAML::Value << rc.model->getFilePath();
    // }

}

void Serializer::saveScene(const std::unique_ptr<core::Scene>& scene, const std::string& filePath) {
    YAML::Emitter out;

    out << YAML::BeginMap;
    out << YAML::Key << "Entities";
    out << YAML::Value << YAML::BeginSeq;
    for (auto ent : scene->group()) {
        out << YAML::BeginMap;
        
        serializeEntity(out, scene, ent);

        out << YAML::EndMap;
    }
    out << YAML::EndSeq << YAML::EndMap;
    
    std::ofstream fout{filePath};
    fout << out.c_str() << '\n';
}

std::unique_ptr<core::Scene> Serializer::loadScene(const std::string& filePath) {
    YAML::Node data;
    try {
        data = YAML::LoadFile(filePath);
    } catch (YAML::ParserException e) {
        std::cout << e.what() << '\n';
        return {};
    }

    std::unique_ptr<core::Scene> scene = std::make_unique<core::Scene>();

    auto entities = data["Entities"];

    for (auto entity : entities) {
        auto ent = scene->createEntity();
        
        if (entity["Tag"]) {
            scene->assign<TagComponent>(ent, entity["Tag"].as<std::string>());
        } else {
            assert(false);
        }
        
        if (entity["TransformComponent"]) {
            auto& tc = scene->assign<TransformComponent>(ent);
            tc.translation = entity["TransformComponent"]["translation"].as<glm::vec3>();
            tc.rotation = entity["TransformComponent"]["rotation"].as<glm::vec3>();
            tc.scale = entity["TransformComponent"]["scale"].as<glm::vec3>();
        }
        
        // if (entity["RenderableComponent"]) {
        //     auto& rc = scene->assign<RenderableComponent>(ent);
        //     rc.model->loadModelFromPath(entity["RenderableComponent"].as<std::string>().c_str());
        // }
    }

    return scene;
}

} // namespace core

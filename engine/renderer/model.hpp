#ifndef MODEL_HPP
#define MODEL_HPP

#include "mesh.hpp"
#include "material.hpp"
#include "../gfx/shaders.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

namespace core {

struct TransformComponent;

} // namespace core


namespace renderer {

class Model {
public:
    Model() = default;
    Model(const std::string& modelPath);
    ~Model();

    Model(Model&& model) = delete;
    Model& operator=(Model&& model) = delete;

    Model(Model& model) = delete;
    Model& operator=(Model& model) = delete;

    const std::string& getFilePath() const { return m_filePath; }

    void loadModelFromPath(const std::string& modelPath);

    void draw(gfx::ShaderProgram& shader, const core::TransformComponent& transform, bool withMaterial = true);

    static void componentPanel(Model& model, event::Dispatcher& dispatcher, ecs::EntityID ent);

private:
    void processNode(aiNode *node, const aiScene *scene, aiMatrix4x4& transform);
    std::unique_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene, aiMatrix4x4& transform);
    std::unique_ptr<Material> processMaterial(aiMaterial *material);
    std::shared_ptr<gfx::Texture> loadMaterialTexture(aiMaterial *mat, aiTextureType type, std::string typeName);

private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::string m_filePath, m_directory;
    std::unordered_map<std::string, std::shared_ptr<gfx::Texture>> m_texturesLoaded;
};

} // namespace renderer

#endif
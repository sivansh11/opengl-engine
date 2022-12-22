#ifndef MODEL_HPP
#define MODEL_HPP

#include "mesh.hpp"
#include "material.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <memory>
#include <unordered_map>

namespace core {

class Model {
public:
    Model() = default;
    Model(const char *modelPath);
    ~Model();

    Model(Model&& model);
    Model& operator=(Model&& model);

    Model(Model& model) = delete;
    Model& operator=(Model& model) = delete;

    void                      loadModelFromPath(const char *modelPath);
    void                      processNode(aiNode *node, const aiScene *scene, aiMatrix4x4& transform);
    std::unique_ptr<Mesh>     processMesh(aiMesh *mesh, const aiScene *scene, aiMatrix4x4& transform);
    std::unique_ptr<Material> processMaterial(aiMaterial *material);

    std::shared_ptr<gfx::Texture2D> loadMaterialTexture(aiMaterial *mat, aiTextureType type, std::string typeName);

    void draw(gfx::ShaderProgram& shader, bool withMaterial = true);

    static void componentPanel(Model& model, event::Dispatcher& dispatcher, ecs::EntityID ent);

private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::string m_filePath, m_directory;
    std::unordered_map<std::string, std::shared_ptr<gfx::Texture2D>> texturesLoaded;
};

} // namespace core

#endif
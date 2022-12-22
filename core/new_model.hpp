#ifndef NEW_MODEL_HPP
#define NEW_MODEL_HPP

#include "new_mesh.hpp"
#include "new_material.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <memory>
#include <unordered_map>

namespace core {

class NewModel {
public:
    NewModel() = default;
    NewModel(const std::string& modelPath);
    ~NewModel();

    void loadModelFromPath(const std::string& modelPath);
    void                      processNode(aiNode *node, const aiScene *scene, aiMatrix4x4& transform);
    std::unique_ptr<NewMesh>     processMesh(aiMesh *mesh, const aiScene *scene, aiMatrix4x4& transform);
    std::unique_ptr<NewMaterial> processMaterial(aiMaterial *material);

private:
    std::vector<std::unique_ptr<NewMesh>> m_meshes;
    std::string m_filePath, m_directory;
    std::unordered_map<std::string, std::shared_ptr<gfx::Texture2D>> m_texturesLoaded;
};

} // namespace core

#endif
#include "new_model.hpp"

namespace core {

NewModel::NewModel(const std::string& modelPath) {
    loadModelFromPath(modelPath);
}

void NewModel::loadModelFromPath(const std::string& modelPath) {
    m_meshes.clear();
    m_texturesLoaded.clear();
    m_filePath = modelPath;
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(modelPath,
                                             aiProcess_Triangulate |
                                             aiProcess_GenUVCoords |
                                             aiProcess_GenNormals  

                                             );
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::runtime_error(importer.GetErrorString());
    }

}

} // namespace core

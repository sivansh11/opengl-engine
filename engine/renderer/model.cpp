#include "model.hpp"

#include "../core/components.hpp"

namespace renderer {

Model::Model(const std::string& modelPath) {
    loadModelFromPath(modelPath);
}

Model::~Model() {

}

void Model::loadModelFromPath(const std::string& modelPath) {
    m_meshes.clear();
    m_texturesLoaded.clear();
    m_filePath = modelPath;
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(modelPath,
                                             aiProcess_Triangulate |
                                             aiProcess_GenNormals
                                            );
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::runtime_error(importer.GetErrorString());
    }

    m_directory = m_filePath.substr(0, m_filePath.find_last_of('/'));
    aiMatrix4x4 transform{};
    processNode(scene->mRootNode, scene, transform);
}

void Model::processNode(aiNode *node, const aiScene *scene, aiMatrix4x4 &transform) {
    aiMatrix4x4 localTransform = transform * node->mTransformation;
    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, localTransform);
    }
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene, localTransform));
    }
}

std::unique_ptr<Mesh> Model::processMesh(aiMesh *mesh, const aiScene *scene, aiMatrix4x4 &transform) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(mesh->mNumVertices);
    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        vertex.pos.x = mesh->mVertices[i].x;
        vertex.pos.y = mesh->mVertices[i].y;
        vertex.pos.z = mesh->mVertices[i].z;

        vertex.nor.x = mesh->mNormals[i].x;
        vertex.nor.y = mesh->mNormals[i].y;
        vertex.nor.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0]) {
            vertex.uv.x = mesh->mTextureCoords[0][i].x;
            vertex.uv.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.uv = {0, 0};
        }

        vertices.push_back(vertex);
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace& face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    auto processedMaterial = processMaterial(scene->mMaterials[mesh->mMaterialIndex]);
    auto processedMesh = std::make_unique<Mesh>(vertices, indices);
    transform.Decompose(*(aiVector3D*)(&processedMesh->m_transform.scale), *(aiVector3D*)(&processedMesh->m_transform.rotation), *(aiVector3D*)(&processedMesh->m_transform.translation));
    processedMesh->material = std::move(processedMaterial);
    return processedMesh;
}

std::unique_ptr<Material> Model::processMaterial(aiMaterial *material) {
    static Material::MaterialTypeBuilder builder = Material::MaterialTypeBuilder()
                                                        .addSampler2D("material.diffuseMap")
                                                        .addSampler2D("material.normalMap")
                                                        .addSampler2D("material.specularMap")
                                                        // .addVec3("material.ambient")
                                                        .addVec3("material.diffuse")
                                                        .addFloat("material.specular")
                                                        // .addFloat("material.shininess")
                                                        .addBool("material.hasDiffuseMap")
                                                        .addBool("material.hasNormalMap")
                                                        .addBool("material.hasSpecularMap")
                                                        ;
    std::unique_ptr<Material> mat(builder.build());

    auto diffuse = loadMaterialTexture(material, aiTextureType_DIFFUSE, "diffuse");
    auto normal = loadMaterialTexture(material, aiTextureType_NORMALS, "normal");
    auto specular = loadMaterialTexture(material, aiTextureType_SPECULAR, "specular");
    
    aiColor4D color;

    // if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &color)) {
    //     mat->assign("material.ambient", glm::vec3(color.r, color.g, color.b));
    // } else {
    //     mat->assign("material.ambient", glm::vec3(1, 1, 1));
    // }

    if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color)) {
        mat->assign("material.diffuse", glm::vec3(color.r, color.g, color.b));
    } else {
        mat->assign("material.diffuse", glm::vec3(1, 1, 1));
    }

    if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color)) {
        mat->assign("material.specular", float((color.r + color.g + color.b) / 3));
    } else {
        mat->assign("material.specular", float(1));
    }

    if (material->GetTextureCount(aiTextureType_DIFFUSE) == 0) {
        mat->assign("material.hasDiffuseMap", false);
    } else {
        mat->assign("material.hasDiffuseMap", true);
    }

    if (material->GetTextureCount(aiTextureType_SPECULAR) == 0) {
        mat->assign("material.hasSpecularMap", false);
    } else {
        mat->assign("material.hasSpecularMap", true);
    }

    if (material->GetTextureCount(aiTextureType_NORMALS) == 0) {
        mat->assign("material.hasNormalMap", false);
    } else {
        mat->assign("material.hasNormalMap", true);
    }    

    // float shininess;
    // if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess) && shininess > 0) {
    //     mat->assign("material.shininess", shininess);
    // } else {
    //     mat->assign("material.shininess", 32.0f);
    // }

    mat->assign("material.diffuseMap", diffuse);
    mat->assign("material.normalMap", normal);
    mat->assign("material.specularMap", specular);
    
    return mat;
}

std::shared_ptr<gfx::Texture2D> Model::loadMaterialTexture(aiMaterial *mat, aiTextureType type, std::string typeName) {
    if (mat->GetTextureCount(type) == 0) {
        unsigned char data[4];
        for (int i = 0; i < 4; i++) 
            data[i] = std::numeric_limits<unsigned char>::max();
        std::shared_ptr<gfx::Texture2D> tex = std::make_shared<gfx::Texture2D>(data, GL_RGBA);
        return tex;
    }
    if (mat->GetTextureCount(type) == 0 && typeName == "normal") {
        unsigned char data[3];
        data[0] = std::numeric_limits<unsigned char>::max() / 2;
        data[1] = std::numeric_limits<unsigned char>::max() / 2;
        data[2] = std::numeric_limits<unsigned char>::max();
        std::shared_ptr<gfx::Texture2D> tex = std::make_shared<gfx::Texture2D>(data, GL_RGB);
        return tex;
    }
    if (mat->GetTextureCount(type) == 0 && typeName == "specular") {
        unsigned char data[1] = {std::numeric_limits<unsigned char>::max() / 2};
        std::shared_ptr<gfx::Texture2D> tex = std::make_shared<gfx::Texture2D>(data, GL_RED);
        return tex;
    }

    if (mat->GetTextureCount(type) == 0) return nullptr;

    std::shared_ptr<gfx::Texture2D> tex;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        if (m_texturesLoaded.contains(str.C_Str())) {
            return m_texturesLoaded.at(str.C_Str());
        }
        tex = std::make_shared<gfx::Texture2D>((m_directory + '/' + str.C_Str()).c_str());
        m_texturesLoaded[str.C_Str()] = tex;
    }
    return tex;
}

void Model::draw(gfx::ShaderProgram& shader, const core::TransformComponent& transform, bool withMaterial) {
    for (auto& mesh : m_meshes) {
        if (withMaterial)
            mesh->material->bind(shader);
        mesh->draw(shader, transform);
    }
}

} // namespace renderer

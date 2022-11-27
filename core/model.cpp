#include "model.hpp"

#include <iostream>
#include <limits>

namespace core {

Model::Model(const char *modelPath) {
    loadModelFromPath(modelPath);
}

Model::~Model() {

}

Model::Model(Model&& model) {
    m_meshes = std::move(model.m_meshes);
    m_filePath = std::move(model.m_filePath);
    m_directory = std::move(model.m_directory);
    texturesLoaded = std::move(model.texturesLoaded);

    // model.m_meshes = nullptr;
    model.m_filePath = {};
    model.m_directory = {};
    model.texturesLoaded = {};
}

Model& Model::operator=(Model&& model) {
    m_meshes = std::move(model.m_meshes);
    m_filePath = std::move(model.m_filePath);
    m_directory = std::move(model.m_directory);
    texturesLoaded = std::move(model.texturesLoaded);

    model.m_meshes.clear();
    model.m_filePath = {};
    model.m_directory = {};
    model.texturesLoaded = {};

    return *this;
}


void Model::loadModelFromPath(const char *modelPath) {
    m_meshes.clear();
    texturesLoaded.clear();
    m_filePath = modelPath;
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(modelPath,
                                             aiProcess_Triangulate |
                                            //  aiProcess_FlipUVs     |
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_GenNormals);
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

        assert(mesh->HasTangentsAndBitangents());

        vertex.tangent.x = mesh->mTangents[i].x;
        vertex.tangent.y = mesh->mTangents[i].y;
        vertex.tangent.z = mesh->mTangents[i].z;

        vertex.biTangent.x = mesh->mBitangents[i].x;
        vertex.biTangent.y = mesh->mBitangents[i].y;
        vertex.biTangent.z = mesh->mBitangents[i].z;


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
    transform.Decompose(*(aiVector3D*)(&processedMesh->transform.scale), *(aiVector3D*)(&processedMesh->transform.rotation), *(aiVector3D*)(&processedMesh->transform.translation));
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
        if (texturesLoaded.contains(str.C_Str())) {
            return texturesLoaded.at(str.C_Str());
        }
        tex = std::make_shared<gfx::Texture2D>((m_directory + '/' + str.C_Str()).c_str());
        texturesLoaded[str.C_Str()] = tex;
    }
    return tex;
}

void Model::draw(gfx::ShaderProgram& shader) {
    for (auto& mesh : m_meshes) {
        mesh->material->bind(shader);
        mesh->draw();
    }
}

void Model::componentPanel(Model& model, event::Dispatcher& dispatcher, ecs::EntityID ent) {
    if (ImGui::BeginDragDropTarget()) {
        const ImGuiPayload *payload = ImGui::GetDragDropPayload();
        if (std::memcmp(payload->DataType, "PATH", 4) != 0) {
            ImGui::EndDragDropTarget();
            return;
        }
        model.loadModelFromPath(std::string{reinterpret_cast<char *>(payload->Data), payload->DataSize}.c_str());
        ImGui::EndDragDropTarget();
    }

    if (model.m_filePath.empty()) {
        ImGui::Text("Empty Model");
    } else {
        ImGui::Text("%s", model.m_filePath.c_str());
    }

    if (ImGui::Button("Clear model")) {
        model = {};
    }
    
    // if (ImGui::)
}

} // namespace core

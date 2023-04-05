// #include "model.hpp"

// #include "../core/components.hpp"

// namespace renderer {

// Model::Model(const std::string& modelPath) {
//     loadModelFromPath(modelPath);
// }

// Model::~Model() {

// }

// void Model::loadModelFromPath(const std::string& modelPath) {
//     m_meshes.clear();
//     m_texturesLoaded.clear();
//     m_filePath = modelPath;
//     Assimp::Importer importer;
//     const aiScene *scene = importer.ReadFile(modelPath,
//                                              aiProcess_Triangulate |
//                                              aiProcess_GenNormals |
//                                              aiProcess_CalcTangentSpace |
//                                              aiProcess_PreTransformVertices
//                                             );
//     if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
//         throw std::runtime_error(importer.GetErrorString());
//     }

//     m_directory = m_filePath.substr(0, m_filePath.find_last_of('/'));
//     aiMatrix4x4 transform{};
//     processNode(scene->mRootNode, scene, transform);
// }

// void Model::processNode(aiNode *node, const aiScene *scene, aiMatrix4x4 &transform) {
//     aiMatrix4x4 localTransform = transform * node->mTransformation;
//     for (uint32_t i = 0; i < node->mNumChildren; i++) {
//         processNode(node->mChildren[i], scene, localTransform);
//     }
//     for (uint32_t i = 0; i < node->mNumMeshes; i++) {
//         aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
//         m_meshes.push_back(processMesh(mesh, scene, localTransform));
//     }
// }

// std::unique_ptr<Mesh> Model::processMesh(aiMesh *mesh, const aiScene *scene, aiMatrix4x4 &transform) {
//     std::vector<Vertex> vertices;
//     std::vector<uint32_t> indices;

//     vertices.reserve(mesh->mNumVertices);
//     for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
//         Vertex vertex;

//         vertex.pos.x = mesh->mVertices[i].x;
//         vertex.pos.y = mesh->mVertices[i].y;
//         vertex.pos.z = mesh->mVertices[i].z;

//         vertex.nor.x = mesh->mNormals[i].x;
//         vertex.nor.y = mesh->mNormals[i].y;
//         vertex.nor.z = mesh->mNormals[i].z;

//         if (mesh->mTextureCoords[0]) {
//             vertex.uv.x = mesh->mTextureCoords[0][i].x;
//             vertex.uv.y = mesh->mTextureCoords[0][i].y;
//         } else {
//             vertex.uv = {0, 0};
//         }

//         if (mesh->HasTangentsAndBitangents()) {
//             // vertex.tangent.x = mesh->mTangents[i].x;
//             // vertex.tangent.y = mesh->mTangents[i].y;
//             // vertex.tangent.z = mesh->mTangents[i].z;
//             // vertex.biTangent.x = mesh->mBitangents[i].x;
//             // vertex.biTangent.y = mesh->mBitangents[i].y;
//             // vertex.biTangent.z = mesh->mBitangents[i].z;
//         } else {
//             // assert(false);
//         }

//         vertices.push_back(vertex);
//     }

//     for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
//         aiFace& face = mesh->mFaces[i];
//         for (uint32_t j = 0; j < face.mNumIndices; j++) {
//             indices.push_back(face.mIndices[j]);
//         }
//     }

//     auto processedMaterial = processMaterial(scene->mMaterials[mesh->mMaterialIndex]);
//     auto processedMesh = std::make_unique<Mesh>(vertices, indices);
//     transform.Decompose(*(aiVector3D*)(&processedMesh->m_transform.scale), *(aiVector3D*)(&processedMesh->m_transform.rotation), *(aiVector3D*)(&processedMesh->m_transform.translation));
//     processedMesh->material = std::move(processedMaterial);
//     return processedMesh;
// }

// std::unique_ptr<Material> Model::processMaterial(aiMaterial *material) {
//     static Material::MaterialTypeBuilder builder = Material::MaterialTypeBuilder()
//                                                             .addVec3("material.diffuseColor")
//                                                             .addVec3("material.specularColor")
//                                                             .addFloat("material.reflectivity")
//                                                             .addFloat("material.emissivity")
//                                                             .addFloat("material.transparency")
//                                                         // .addSampler2D("material.diffuseMap")
//                                                         // .addSampler2D("material.normalMap")
//                                                         // .addSampler2D("material.specularMap")
//                                                         // // .addVec3("material.ambient")
//                                                         // .addVec3("material.diffuse")
//                                                         // .addFloat("material.specular")
//                                                         // .addFloat("material.shininess")
//                                                         // .addBool("material.hasDiffuseMap")
//                                                         // .addBool("material.hasNormalMap")
//                                                         // .addBool("material.hasSpecularMap")
//                                                         ;
//     std::unique_ptr<Material> mat(builder.build());

//     aiColor4D color;
//     if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color)) {
//         mat->assign("material.diffuseColor", glm::vec3{color.r, color.g, color.b});
//     } else {
//         mat->assign("material.diffuseColor", glm::vec3{1, 1, 1});
//     }
//     if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color)) {
//         mat->assign("material.diffuseColor", glm::vec3{color.r, color.g, color.b});
//     } else {
//         mat->assign("material.diffuseColor", glm::vec3{1, 1, 1});
//     }

//     mat->assign("material.reflectivity", float{1});
//     mat->assign("material.emissivity", float{0});
//     mat->assign("material.transparency", float{0});

//     return mat;
// }

// std::shared_ptr<gfx::Texture> Model::loadMaterialTexture(aiMaterial *mat, aiTextureType type, std::string typeName) {
//     if (mat->GetTextureCount(type) == 0 && typeName == "diffuse") {
//         unsigned char data[4];
//         for (int i = 0; i < 4; i++) 
//             data[i] = std::numeric_limits<unsigned char>::max();
//         std::shared_ptr<gfx::Texture> tex = gfx::Texture::Builder().build(gfx::Texture::Type::e2D);
//         gfx::Texture::CreateInfo info{};
//         info.width = 1;
//         info.height = 1;
//         info.internalFormat = gfx::Texture::InternalFormat::eRGBA8;
//         gfx::Texture::UploadInfo uploadInfo{};
//         uploadInfo.data = data;
//         uploadInfo.dataType = gfx::Texture::DataType::eUNSIGNED_BYTE;
//         uploadInfo.format = gfx::Texture::Format::eRGBA;
//         uploadInfo.level = 0;
//         tex->create(info);
//         tex->upload(uploadInfo);

//         return tex;
//     }
//     if (mat->GetTextureCount(type) == 0 && typeName == "normal") {
//         unsigned char data[3];
//         data[0] = std::numeric_limits<unsigned char>::max() / 2;
//         data[1] = std::numeric_limits<unsigned char>::max() / 2;
//         data[2] = std::numeric_limits<unsigned char>::max();
//         // std::shared_ptr<gfx::Texture> tex = std::make_shared<gfx::Texture>(data, GL_RGB);
//         std::shared_ptr<gfx::Texture> tex = gfx::Texture::Builder().build(gfx::Texture::Type::e2D);
//         gfx::Texture::CreateInfo info{};
//         info.width = 1;
//         info.height = 1;
//         info.internalFormat = gfx::Texture::InternalFormat::eRGBA8;
//         gfx::Texture::UploadInfo uploadInfo{};
//         uploadInfo.data = data;
//         uploadInfo.dataType = gfx::Texture::DataType::eUNSIGNED_BYTE;
//         uploadInfo.format = gfx::Texture::Format::eRGB;
//         uploadInfo.level = 0;
//         tex->create(info);
//         tex->upload(uploadInfo);
//         return tex;
//     }
//     if (mat->GetTextureCount(type) == 0 && typeName == "specular") {
//         unsigned char data[1] = {std::numeric_limits<unsigned char>::max() / 2};
//         // std::shared_ptr<gfx::Texture> tex = std::make_shared<gfx::Texture>(data, GL_RED);
//         std::shared_ptr<gfx::Texture> tex = gfx::Texture::Builder().build(gfx::Texture::Type::e2D);
//         gfx::Texture::CreateInfo info{};
//         info.internalFormat = gfx::Texture::InternalFormat::eRGBA8;
//         info.width = 1;
//         info.height = 1;
//         gfx::Texture::UploadInfo uploadInfo{};
//         uploadInfo.data = data;
//         uploadInfo.dataType = gfx::Texture::DataType::eUNSIGNED_BYTE;
//         uploadInfo.format = gfx::Texture::Format::eRED;
//         uploadInfo.level = 0;
//         tex->create(info);
//         tex->upload(uploadInfo);
//         return tex;
//     }

//     if (mat->GetTextureCount(type) == 0) return nullptr;

//     std::shared_ptr<gfx::Texture> tex;
//     gfx::Texture::Builder builder;
//     builder.setMinFilter(gfx::Texture::MinFilter::eLINEAR_MIPMAP_LINEAR);
//     for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
//         aiString str;
//         mat->GetTexture(type, i, &str);
//         if (m_texturesLoaded.contains(str.C_Str())) {
//             return m_texturesLoaded.at(str.C_Str());
//         }
//         tex = builder.build(gfx::Texture::Type::e2D);
//         tex->loadImage((m_directory + '/' + str.C_Str()).c_str());
//         // tex = std::make_shared<gfx::Texture>((m_directory + '/' + str.C_Str()).c_str());
//         m_texturesLoaded[str.C_Str()] = tex;
//     }
//     return tex;
// }

// void Model::draw(gfx::ShaderProgram& shader, const core::TransformComponent& transform, bool withMaterial) {
//     for (auto& mesh : m_meshes) {
//         if (withMaterial)
//             mesh->material->bind(shader);
//         mesh->draw(shader, transform);
//     }
// }

// void Model::componentPanel(Model& model, event::Dispatcher& dispatcher, ecs::EntityID ent) {
//     ImGui::Text("%s", model.getFilePath().c_str()); 
// }

// } // namespace renderer

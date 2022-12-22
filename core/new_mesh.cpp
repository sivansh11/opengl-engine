#include "new_mesh.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace core {

NewMesh::NewMesh(const std::vector<NewVertex>& vertices, const std::vector<uint32_t>& indices) 
 :  m_vertexBuffer(sizeof(NewVertex) * vertices.size()),
    m_indexBuffer(sizeof(uint32_t) * indices.size()) {
    
    m_vertexBuffer.push(vertices.data());
    m_indexBuffer.push(indices.data());

    m_vertexAttribute.attributeLocation(0, 3, offsetof(NewVertex, pos));
    m_vertexAttribute.attributeLocation(1, 3, offsetof(NewVertex, nor));
    m_vertexAttribute.attributeLocation(2, 2, offsetof(NewVertex, uv));

    m_vertexAttribute.bindVertexBuffer<NewVertex>(m_vertexBuffer);
    m_vertexAttribute.bindElementBuffer(m_indexBuffer);

    m_indexCount = indices.size();
}

NewMesh::~NewMesh() {
    m_material = {};
}

void NewMesh::draw(gfx::ShaderProgram& shader, TransformComponent& parentTransform) {
    m_vertexAttribute.bind();
    m_material->bind(shader);
    shader.mat4f("model", glm::value_ptr(parentTransform.mat4() * m_transform.mat4()));
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
}

} // namespace core

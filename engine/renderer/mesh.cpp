#include "mesh.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace renderer {

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) 
    : vertexBuffer(sizeof(Vertex) * vertices.size()),
      indexBuffer(sizeof(uint32_t) * indices.size()) {
    vertexBuffer.push(vertices.data());
    indexBuffer.push(indices.data());
    vertexAttribute.attributeLocation(0, 3, offsetof(Vertex, pos));
    vertexAttribute.attributeLocation(1, 3, offsetof(Vertex, nor));
    vertexAttribute.attributeLocation(2, 2, offsetof(Vertex, uv));

    vertexAttribute.bindVertexBuffer<Vertex>(vertexBuffer);
    vertexAttribute.bindElementBuffer(indexBuffer);

    indexCount = indices.size();
}

Mesh::~Mesh() {

}

void Mesh::draw(gfx::ShaderProgram& program, const core::TransformComponent& transform) {
    vertexAttribute.bind();
    program.mat4f("model", glm::value_ptr(transform.mat4() * m_transform.mat4()));
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

} // namespace renderer

#include "mesh.hpp"


namespace core {

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) 
    : vertexBuffer(sizeof(Vertex) * vertices.size()),
      indexBuffer(sizeof(uint32_t) * indices.size()) {
    vertexBuffer.push(vertices.data());
    indexBuffer.push(indices.data());
    vertexAttribute.attributeLocation(0, 3, offsetof(Vertex, pos));
    vertexAttribute.attributeLocation(1, 3, offsetof(Vertex, nor));
    vertexAttribute.attributeLocation(2, 2, offsetof(Vertex, uv));
    vertexAttribute.attributeLocation(3, 3, offsetof(Vertex, tangent));
    vertexAttribute.attributeLocation(4, 3, offsetof(Vertex, biTangent));

    vertexAttribute.bindVertexBuffer<Vertex>(vertexBuffer);
    vertexAttribute.bindElementBuffer(indexBuffer);

    indexCount = indices.size();
}

Mesh::~Mesh() {

}

Mesh::Mesh(Mesh&& mesh) {
    vertexBuffer = std::move(mesh.vertexBuffer);
    indexBuffer = std::move(mesh.indexBuffer);
    vertexAttribute = std::move(mesh.vertexAttribute);
    indexCount = mesh.indexCount;
    transform = std::move(mesh.transform);
    material = std::move(mesh.material);

    mesh.vertexBuffer = {};
    mesh.indexBuffer = {};
    mesh.vertexAttribute = {};
    mesh.indexCount = {};
    mesh.transform = {};
    mesh.material = {};
}

void Mesh::draw() {
    vertexAttribute.bind();
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

} // namespace core

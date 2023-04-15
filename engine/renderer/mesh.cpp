#include "mesh.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace renderer {

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) 
  : m_vertices(sizeof(Vertex) * vertices.size()),
    m_indices(sizeof(uint32_t) * indices.size()),
    m_vertexAttribute(Vertex::getAttributes()),
    m_indexCount(indices.size()) {
	m_vertices.push(vertices.data());
	m_indices.push(indices.data());
	m_vertexAttribute.bindVertexBuffer<Vertex>(m_vertices);
	m_vertexAttribute.bindElementBuffer(m_indices);
}

Mesh::~Mesh() {

}

void Mesh::draw() {
	m_vertexAttribute.bind();
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);	
}

} // namespace renderer

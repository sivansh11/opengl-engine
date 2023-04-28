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

void Mesh::draw(gfx::ShaderProgram& shader, const core::TransformComponent& transform, bool withMaterial) {
	m_vertexAttribute.bind();
	
	if (withMaterial)
		material->bind(shader);
	shader.mat4f("model", glm::value_ptr(transform.mat4() * m_transform.mat4()));
	shader.mat4f("invModel", glm::value_ptr(glm::inverse(transform.mat4() * m_transform.mat4())));

	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);	
}

} // namespace renderer

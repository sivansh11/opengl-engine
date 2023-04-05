#ifndef MESH_HPP
#define MESH_HPP

#include "material.hpp"
#include "../core/components.hpp"
#include "../gfx/buffer.hpp"
#include "../gfx/shaders.hpp"
#include "../gfx/vertex_attribute.hpp"

#include <glm/glm.hpp>

namespace renderer {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    static gfx::VertexAttribute getAttributes() {
        gfx::VertexAttribute vertexAttribute{};
        vertexAttribute.attributeLocation(0, 3, offsetof(Vertex, position));
        vertexAttribute.attributeLocation(1, 3, offsetof(Vertex, normal));
        vertexAttribute.attributeLocation(2, 2, offsetof(Vertex, uv));
        return vertexAttribute;
    } 
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    ~Mesh();

    Mesh(const Mesh& mesh) = delete;
    Mesh& operator=(const Mesh& mesh) = delete;

    void bind() const;

private:
    gfx::Buffer m_vertices;
    gfx::Buffer m_indices;
    gfx::VertexAttribute m_vertexAttribute;
    uint32_t m_indexCount;
};

} // namespace renderer

#endif
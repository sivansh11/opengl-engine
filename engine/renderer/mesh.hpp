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
    glm::vec3 tangent;
    glm::vec3 biTangent;
    static gfx::VertexAttribute getAttributes() {
        gfx::VertexAttribute vertexAttribute{};
        vertexAttribute.attributeLocation(0, 3, offsetof(Vertex, position));
        vertexAttribute.attributeLocation(1, 3, offsetof(Vertex, normal));
        vertexAttribute.attributeLocation(2, 2, offsetof(Vertex, uv));
        vertexAttribute.attributeLocation(3, 3, offsetof(Vertex, tangent));
        vertexAttribute.attributeLocation(4, 3, offsetof(Vertex, biTangent));
        return vertexAttribute;
    } 
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    ~Mesh();

    Mesh(const Mesh& mesh) = delete;
    Mesh& operator=(const Mesh& mesh) = delete;

    void draw(gfx::ShaderProgram& shader, const core::TransformComponent& transform, bool withMaterial = true);
    
    core::TransformComponent m_transform;
    std::unique_ptr<Material> material;

private:
    gfx::Buffer m_vertices;
    gfx::Buffer m_indices;
    gfx::VertexAttribute m_vertexAttribute;
    uint32_t m_indexCount;
};

} // namespace renderer

#endif
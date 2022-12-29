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
    glm::vec3 pos;
    glm::vec3 nor;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 biTangent;
};

class Mesh {
public:
    Mesh() = default;
    Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    ~Mesh();
    Mesh(Mesh&& mesh) = delete;
    Mesh& operator=(Mesh&& mesh) = delete;

    Mesh(Mesh& mesh) = delete;
    Mesh& operator=(Mesh& mesh) = delete;

    void draw(gfx::ShaderProgram& shaderProgram, const core::TransformComponent& transform);

private:
    gfx::Buffer vertexBuffer;
    gfx::Buffer indexBuffer;
    gfx::VertexAttribute vertexAttribute;
    uint32_t indexCount;
    core::TransformComponent m_transform{};
    std::shared_ptr<Material> material;

    friend class Model;
};

} // namespace renderer

#endif
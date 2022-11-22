#ifndef MESH_HPP
#define MESH_HPP

#include "gfx/vertex_attribute.hpp"
#include "gfx/buffer.hpp"
#include "material.hpp"
#include "transform.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace core {

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
    Mesh(Mesh&& mesh);
    Mesh& operator=(Mesh&& mesh);

    Mesh(Mesh& mesh) = delete;
    Mesh& operator=(Mesh& mesh) = delete;

    void draw();

private:
    gfx::Buffer vertexBuffer;
    gfx::Buffer indexBuffer;
    gfx::VertexAttribute vertexAttribute;
    uint32_t indexCount;
    Transform transform{};
    std::shared_ptr<Material> material;

    friend class Model;
};

} // namespace core

#endif
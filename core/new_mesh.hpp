#ifndef NEW_MESH_HPP
#define NEW_MESH_HPP

#include "gfx/vertex_attribute.hpp"
#include "gfx/buffer.hpp"
#include "gfx/shaders.hpp"
#include "new_material.hpp"

#include "components/transform.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace core {

struct NewVertex {
    glm::vec3 pos;
    glm::vec3 nor;
    glm::vec2 uv;
};

class NewMesh {
public:
    NewMesh() = default;
    NewMesh(const std::vector<NewVertex>& vertices, const std::vector<uint32_t>& indices);
    ~NewMesh();

    void draw(gfx::ShaderProgram& shader, TransformComponent& parentTransform);

private:
    gfx::Buffer m_vertexBuffer;
    gfx::Buffer m_indexBuffer;
    gfx::VertexAttribute m_vertexAttribute;
    uint32_t m_indexCount;
    TransformComponent m_transform{};
    std::shared_ptr<NewMaterial> m_material;
    
};

} // namespace core

#endif 
#ifndef BVH_HPP
#define BVH_HPP

#include <glm/glm.hpp>

#include <vector>

namespace core {

namespace bvh {

// uint32_t expandBits(uint32_t v) {
//     v = (v * 0x00010001u) & 0xFF0000FFu;
//     v = (v * 0x00000101u) & 0x0F00F00Fu;
//     v = (v * 0x00000011u) & 0xC30C30C3u;
//     v = (v * 0x00000005u) & 0x49249249u;
//     return v;
// }

// uint32_t mortonCode(glm::vec3 v) {
//     v.x = glm::min(glm::max(v.x * 1024.0f, 0.0f), 1023.0f);
//     v.y = glm::min(glm::max(v.y * 1024.0f, 0.0f), 1023.0f);
//     v.z = glm::min(glm::max(v.z * 1024.0f, 0.0f), 1023.0f);
//     uint32_t xx = expandBits(static_cast<uint32_t>(v.x));
//     uint32_t yy = expandBits(static_cast<uint32_t>(v.y));
//     uint32_t zz = expandBits(static_cast<uint32_t>(v.z));
//     return xx * 4 + yy * 2 + zz;
// }

struct Triangle {
    Triangle() = default;
    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) : vert0(v0), vert1(v1), vert2(v2) {}
    alignas(16) glm::vec3 vert0;
    alignas(16) glm::vec3 vert1;
    alignas(16) glm::vec3 vert2;

    void boundingBox(glm::vec3 &min, glm::vec3 &max) {
        min = glm::min(min, vert0);
        min = glm::min(min, vert1);
        min = glm::min(min, vert2);
        max = glm::max(max, vert0);
        max = glm::max(max, vert1);
        max = glm::max(max, vert2);
    }

    glm::vec3 centroid(){
        return (vert0 + vert1 + vert2) / 3.0f;
    }
};

struct BvhNode {
    glm::vec3 aabbMin;
    uint32_t leftFirst;
    glm::vec3 aabbMax;
    uint32_t triangleCount;
};

bool bvhNodeIsLeaf(BvhNode& node) {
    return node.triangleCount > 0;
}


class Bvh {
    struct GpuBufferData {
        Triangle *triangleBuffer;
        uint32_t triangleCount;
        uint32_t *idBuffer;
        BvhNode  *bvhNodeBuffer;
    };

public:
    Bvh() = default;

    // void builder(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
    //     std::vector<Triangle> triangles;
    //     assert(indices.size() % 3 == 0);
    //     for (int i = 0; i < indices.size(); i+=3) {
    //         triangles.push_back(Triangle{vertices[i].pos, vertices[i + 1].pos, vertices[i + 2].pos});
    //     }   
    //     builder(triangles);
    // }


    void builder(std::vector<Triangle>& triangles) {
        m_triangles = &triangles;
        buildBvh();
    }

    GpuBufferData getGpuBufferData() {
        return {m_triangles->data(), static_cast<uint32_t>(m_triangles->size()), m_triangleIDs.data(), m_bvhNodes.data()};
    }

private:

    void updateNodeBound(uint32_t nodeID) {
        BvhNode& node = m_bvhNodes[nodeID];
        node.aabbMin = glm::vec3{std::numeric_limits<float>::max()};
        node.aabbMax = glm::vec3{-std::numeric_limits<float>::max()};
        for (int first = node.leftFirst, i = 0; i < node.triangleCount; i++) {
            uint32_t leafID = m_triangleIDs[first + i];
            Triangle& leaf = m_triangles->at(leafID);
            leaf.boundingBox(node.aabbMin, node.aabbMax);
        }
    }

    void subDivideBvh(uint32_t nodeID) {
        BvhNode& node = m_bvhNodes[nodeID];
        if (node.triangleCount <= 2) return;
        glm::vec3 e = node.aabbMax - node.aabbMin;
        int axis = 0;
        if (e.y > e.x) axis = 1;
        if (e.z > e[axis]) axis = 2;
        float splitPos = node.aabbMin[axis] + e[axis] * 0.5f;

        int i = node.leftFirst;
        int j = i + node.triangleCount - 1;

        while (i <= j) {
            if (m_triangles->at(m_triangleIDs[i]).centroid()[axis] < splitPos)
                i++;
            else 
                std::swap(m_triangleIDs[i], m_triangleIDs[j--]);
        }
        uint32_t leftCount = i - node.leftFirst;
        if (leftCount == 0 || leftCount == node.triangleCount) return;

        uint32_t leftChildID = m_nodeUsed++;
        uint32_t rightChildID = m_nodeUsed++;

        m_bvhNodes[leftChildID].leftFirst = node.leftFirst;
        m_bvhNodes[leftChildID].triangleCount = leftCount;
        m_bvhNodes[rightChildID].leftFirst = i;
        m_bvhNodes[rightChildID].triangleCount = node.triangleCount - leftCount;
        node.leftFirst = leftChildID;
        node.triangleCount = 0;

        updateNodeBound(leftChildID);
        updateNodeBound(rightChildID);

        subDivideBvh(leftChildID);
        subDivideBvh(rightChildID);
    }

    void buildBvh() {
        m_triangleIDs.resize(m_triangles->size());
        for (int i = 0; i < m_triangles->size(); i++) m_triangleIDs[i] = i;
        m_bvhNodes.resize(m_triangles->size() * 2);
        BvhNode& rootNode = m_bvhNodes[m_rootNodeID];
        rootNode.leftFirst = 0;
        rootNode.triangleCount = m_triangles->size();
        updateNodeBound(m_rootNodeID);
        subDivideBvh(m_rootNodeID);
    }

private:
    std::vector<Triangle> *m_triangles;
    std::vector<uint32_t> m_triangleIDs;
    std::vector<BvhNode> m_bvhNodes;
    uint32_t m_rootNodeID = 0;
    uint32_t m_nodeUsed = 1;
};

std::ostream& operator<<(std::ostream& out, const glm::vec3& vec) {
    return out << vec.x << ' ' << vec.y << ' ' << vec.z;
}

std::ostream& operator<<(std::ostream& out, const core::bvh::Triangle& tri) {
    out << tri.vert0 << "  " << tri.vert1 << "  " << tri.vert2;
    return out;
}

std::ostream& operator<<(std::ostream& out, const core::bvh::BvhNode& node) {
    out << node.aabbMin << "  " << node.leftFirst << "  " << node.aabbMax << "  " << node.triangleCount;
    return out;
}

} // namespace bvh

} // namespace core

#endif
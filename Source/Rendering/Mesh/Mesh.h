#pragma once
#include <Rendering/Vertex/Vertex.h>
#include <Rendering/Buffer/Buffer.h>
#include <Common/IdTypes.h>
#include <vector>
#include <unordered_map>
#include <glm/mat4x4.hpp>

namespace MagicRed::Rendering
{
    struct CPUMesh {
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;
        glm::mat4x4 m_transform{0.0};
        bool partiallyTransparentMaterial{false};
    };

    struct GPUMesh {
        AllocatedBuffer vertexBuffer;
        AllocatedBuffer  indexBuffer;
        uint32_t indexCount;
        MaterialId m_materialId{NULL_MATERIAL_ID};

        void cleanup(VmaAllocator allocator);
    };
}
#pragma once
#include <Rendering/Vertex/Vertex.h>
#include <Rendering/Buffer/Buffer.h>
#include <Common/IdTypes.h>
#include <vector>
#include <unordered_map>
#include <Common/Math/Matrix4f.h>

namespace MagicRed::Rendering
{
    struct CPUMesh {
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;
        Matrix4f m_transform;
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
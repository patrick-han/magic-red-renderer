#pragma once
#include <vulkan/vulkan.h>
#include "DefaultPushConstants.h"
#include <span>
#include <Common/IdTypes.h>
#include <Common/Math/Matrix4f.h>

namespace MagicRed::Rendering
{
    class MeshCache;

    struct RenderMeshComponent {
        RenderMeshComponent(const GPUMeshId _GPUmeshId, const MeshCache& _meshCache, Matrix4f _transformMatrix);
        void bind_mesh_buffers_and_draw(VkCommandBuffer commandBuffer, std::span<VkDescriptorSet const> descriptorSets) const;


        
    private:
        const GPUMeshId m_GPUmeshId;
        const MeshCache &m_meshCache;
    public:
        Matrix4f m_transformMatrix;
        MaterialId m_materialId;
    };
}
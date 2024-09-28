#include "ShadowMapStage.h"
#include <Rendering/Core/GfxDevice.h>
#include <Rendering/Mesh/RenderMeshComponent.h>
#include <Rendering/Core/RenderingDefaults.h>

namespace MagicRed::Rendering
{
    ShadowMapStage::ShadowMapStage(
            const GfxDevice& _gfxDevice,
            const VkPipelineRenderingCreateInfoKHR* _pipelineRenderingCreateInfo
        )
        : StageBase(_gfxDevice)
        , m_pipeline(m_gfxDevice)
        {

            VertexInputDescription vertexDescription = VertexInputDescription::get_default_vertex_description();
            m_pipeline.BuildPipeline(
                _pipelineRenderingCreateInfo
                , m_vertexShaderPath, m_fragmentShaderPath
                , vertexDescription
                , m_pushConstantRanges
                , {}
                , m_extent
                , false
                );
        }

    ShadowMapStage::~ShadowMapStage() {}

    void ShadowMapStage::Draw(VkCommandBuffer cmdBuffer, VkDeviceAddress sceneDataBufferAddress, std::span<RenderMeshComponent> renderMeshComponents) {

        vkCmdSetViewport(cmdBuffer, 0, 1, &DEFAULT_VIEWPORT_SHADOWMAP);
        vkCmdSetScissor(cmdBuffer, 0, 1, &DEFAULT_SCISSOR_SHADOWMAP);

        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.GetPipelineHandle());

        for(const RenderMeshComponent& renderMeshComponent : renderMeshComponents)
        {
            ShadowMapPushConstants pushConstants;
            pushConstants.model = renderMeshComponent.m_transformMatrix;
            pushConstants.sceneDataBufferAddress = sceneDataBufferAddress;
            vkCmdPushConstants(cmdBuffer, m_pipeline.GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(pushConstants), &pushConstants);

            renderMeshComponent.bind_mesh_buffers_and_draw(cmdBuffer, std::span<const VkDescriptorSet>());
        }
    }

    void ShadowMapStage::Cleanup() {
        vkDestroyPipelineLayout(m_gfxDevice, m_pipeline.GetPipelineLayout(), nullptr);
        vkDestroyPipeline(m_gfxDevice, m_pipeline.GetPipelineHandle(), nullptr);
    }
}
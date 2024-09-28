#pragma once
#include <Rendering/Mesh/DefaultPushConstants.h>
#include <Rendering/Pipeline/GraphicsPipeline.h>
#include <Common/IdTypes.h>
#include <Rendering/Core/RenderingConfig.h>
#include <Rendering/RenderStages/StageBase.h>
#include <array>

namespace MagicRed::Rendering
{
    class GfxDevice;
    struct RenderMeshComponent;

    class ShadowMapStage final : public StageBase {

        static constexpr std::array<VkPushConstantRange, 1> m_pushConstantRanges = {ShadowMapPushConstants::range()};

    public:
        // ShadowMapStage() = delete;
        ShadowMapStage(
            const GfxDevice& _gfxDevice,
            const VkPipelineRenderingCreateInfoKHR* _pipelineRenderingCreateInfo
        );
        ~ShadowMapStage();
        ShadowMapStage(const ShadowMapStage&) = delete;
        ShadowMapStage& operator=(const ShadowMapStage&) = delete;

        void Draw(VkCommandBuffer cmdBuffer, VkDeviceAddress sceneDataBufferAddress, std::span<RenderMeshComponent> renderMeshComponents);
        void Cleanup() override;

    private:
        const std::string m_vertexShaderPath = std::string("Shaders/shadowmap.vert.spv");
        const std::string m_fragmentShaderPath = std::string("Shaders/shadowmap.frag.spv");
        const VkExtent2D m_extent = {SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION};
    public:
        GraphicsPipeline m_pipeline;
    };
}
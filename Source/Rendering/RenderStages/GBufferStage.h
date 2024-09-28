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

    class GBufferStage final : public StageBase {

        static constexpr std::array<VkPushConstantRange, 1> m_pushConstantRanges = {DefaultPushConstants::range()};

    public:
        GBufferStage() = delete;
        GBufferStage(
            const GfxDevice& _gfxDevice,
            const VkPipelineRenderingCreateInfoKHR* _pipelineRenderingCreateInfo,
            const VkDescriptorSetLayout _bindlessDescriptorSetLayout,
            const VkDescriptorSet _bindlessDescriptorSet
        );
        ~GBufferStage();
        GBufferStage(const GBufferStage&) = delete;
        GBufferStage& operator=(const GBufferStage&) = delete;

        void Draw(VkCommandBuffer cmdBuffer, VkDeviceAddress sceneDataBufferAddress, std::span<RenderMeshComponent> renderMeshComponents);
        void Cleanup() override;

    private:
        const std::string m_vertexShaderPath = std::string("Shaders/triangle_mesh.vert.spv");
        const std::string m_fragmentShaderPath = std::string("Shaders/gbuffer.frag.spv");
        const VkDescriptorSet m_bindlessDescriptorSet;
        const VkExtent2D m_extent = {WINDOW_WIDTH, WINDOW_HEIGHT};
    public:
        GraphicsPipeline m_pipeline;
    };
}
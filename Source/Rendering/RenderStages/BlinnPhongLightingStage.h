#pragma once
#include <Rendering/Mesh/DefaultPushConstants.h>
#include <Rendering/Pipeline/GraphicsPipeline.h>
#include <Common/IdTypes.h>
#include <Rendering/Core/RenderingConfig.h>
#include <Rendering/RenderStages/StageBase.h>
#include <array>
#include <Common/IdTypes.h>

namespace MagicRed::Rendering
{
    class GfxDevice;
    class RenderTextureCache;

    struct DescriptorSetLayoutBinding {
        VkDescriptorType descriptorType;
        int descriptorCount;
    };

    class BlinnPhongLightingStage final : public StageBase {

        static constexpr std::array<VkPushConstantRange, 1> m_pushConstantRanges = {DefaultPushConstants::range()};

    public:
        // BlinnPhongLightingStage() = delete;
        BlinnPhongLightingStage(
            const GfxDevice& _gfxDevice,
            const VkPipelineRenderingCreateInfoKHR* _pipelineRenderingCreateInfo,
            const RenderTextureCache& _renderTextureCache,
            const VkDescriptorPool _globalDescriptorPool,
            const VkDescriptorSetLayout _bindlessDescriptorSetLayout,
            const VkDescriptorSet _bindlessDescriptorSet,
            GPUTextureId _albedoRTId,
            GPUTextureId _worldNormalsRTId,
            GPUTextureId _metallicRoughnessRTId
        );
        ~BlinnPhongLightingStage();
        BlinnPhongLightingStage(const BlinnPhongLightingStage&) = delete;
        BlinnPhongLightingStage& operator=(const BlinnPhongLightingStage&) = delete;

        void Draw(VkCommandBuffer cmdBuffer, VkDeviceAddress sceneDataBufferAddress);
        void Cleanup() override;

    private:
        const std::string m_vertexShaderPath = std::string("Shaders/fullscreen_effect.vert.spv");
        const std::string m_fragmentShaderPath = std::string("Shaders/blinn-phong.frag.spv");
        const VkExtent2D m_extent = {WINDOW_WIDTH, WINDOW_HEIGHT};

        const RenderTextureCache& m_renderTextureCache;
        const VkDescriptorPool m_globalDescriptorPool;
        const VkDescriptorSet m_bindlessDescriptorSet;
        VkDescriptorSetLayout m_lightingDescriptorSetLayout;
        VkDescriptorSet m_lightingDescriptorSet;
    public:
        GraphicsPipeline m_pipeline;
    };
}

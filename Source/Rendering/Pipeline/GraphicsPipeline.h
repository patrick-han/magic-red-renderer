#pragma once
#include <span>
#include <string>
#include <Rendering/Vertex/VertexDescriptors.h>

namespace MagicRed::Rendering
{
    class GfxDevice;

    class GraphicsPipeline {
    public:
        GraphicsPipeline(const GfxDevice& _gfxDevice);
        void BuildPipeline(
            const VkPipelineRenderingCreateInfoKHR* pipelineRenderingCreateInfo
            , const std::string& vertexShaderPath
            , const std::string& fragmentShaderPath
            , VertexInputDescription& vertexDescription
            , std::span<VkPushConstantRange const> pushConstantRanges
            , std::span<VkDescriptorSetLayout const> descriptorSetLayouts
            , VkExtent2D extent
            , bool blendEnable
            );
        ~GraphicsPipeline() = default;
        const VkPipeline& GetPipelineHandle() const;
        const VkPipelineLayout& GetPipelineLayout() const;
        GraphicsPipeline(const GraphicsPipeline&) = delete;
        GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
    private:
        void CreatePipelineLayout(
            std::span<VkPushConstantRange const> pushConstantRanges, 
            std::span<VkDescriptorSetLayout const> descriptorSetLayouts
            );

        const VkDevice m_logicalDevice;
        VkPipeline m_pipeline;
        VkPipelineLayout m_pipelineLayout;
    };
}
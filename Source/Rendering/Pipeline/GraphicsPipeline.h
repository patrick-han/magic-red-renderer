#pragma once
#include <span>
#include <string>
#include <Rendering/Vertex/VertexDescriptors.h>

namespace MagicRed::Rendering
{
    class GfxDevice;
    class GraphicsPipeline;

    class GraphicsPipelineBuilder {
    public:
        GraphicsPipelineBuilder(const GfxDevice& _gfxDevice);
        GraphicsPipelineBuilder& SetRenderingInfo(const VkPipelineRenderingCreateInfoKHR* info);
        GraphicsPipelineBuilder& SetShaders(const std::string& vertexPath, const std::string& fragmentPath);
        GraphicsPipelineBuilder& SetVertexDescription(const VertexInputDescription& description);
        GraphicsPipelineBuilder& SetPushConstantRanges(std::span<VkPushConstantRange const> ranges);
        GraphicsPipelineBuilder& SetDescriptorSetLayouts(std::span<VkDescriptorSetLayout const> layouts);
        GraphicsPipelineBuilder& SetExtent(VkExtent2D extent);
        GraphicsPipelineBuilder& SetBlendEnable(bool enable);
        GraphicsPipelineBuilder& SetCullMode(VkCullModeFlags cullMode);

        GraphicsPipeline Build();
    private:
        VkPipelineLayout CreatePipelineLayout(
            std::span<VkPushConstantRange const> pushConstantRanges, 
            std::span<VkDescriptorSetLayout const> descriptorSetLayouts
        );

        const GfxDevice& m_gfxDevice;
        VkPipelineRenderingCreateInfoKHR m_pipelineRenderingCreateInfo;
        std::string m_vertexShaderPath;
        std::string m_fragmentShaderPath;
        VertexInputDescription m_vertexDescription;
        std::span<VkPushConstantRange const> m_pushConstantRanges;
        std::span<VkDescriptorSetLayout const> m_descriptorSetLayouts;
        VkExtent2D m_extent;
        bool m_blendEnable;
        VkCullModeFlags m_cullMode;
    };

    class GraphicsPipeline {
    public:
        GraphicsPipeline();
        GraphicsPipeline(VkPipeline _pipeline, VkPipelineLayout _pipelineLayout);
        ~GraphicsPipeline() = default;

        static GraphicsPipelineBuilder CreateBuilder(const GfxDevice& _gfxDevice);
        const VkPipeline& GetPipelineHandle() const;
        const VkPipelineLayout& GetPipelineLayout() const;
        GraphicsPipeline(const GraphicsPipeline&) = default;
        GraphicsPipeline& operator=(const GraphicsPipeline&) = default;
    private:
        VkPipeline m_pipeline;
        VkPipelineLayout m_pipelineLayout;
    };
}
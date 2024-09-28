#include "BindlessManager.h"
#include <Rendering/Core/GfxDevice.h>
#include <Rendering/Texture/TextureCache.h>

namespace MagicRed::Rendering {
    BindlessManager::BindlessManager(GfxDevice& device, TextureCache& textureCache)
        : m_GfxDevice(device)
        , m_TextureCache(textureCache)
    {
        
    }

    BindlessManager::~BindlessManager() {
    }

    void BindlessManager::Cleanup() {
        vkDestroyDescriptorSetLayout(m_GfxDevice, m_bindlessDescriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(m_GfxDevice, m_bindlessDescriptorPool, nullptr);
    }

    void BindlessManager::InitBindlessDescriptors() {
        constexpr uint32_t maxBindlessResourceCount = 16536; // Requires MVK_CONFIG_USE_METAL_ARGUMENT_BUFFERS
        constexpr uint32_t maxSamplerCount = 2;

        // Create a global descriptor pool, and let it know how many of each descriptor type we want up front
        std::array<VkDescriptorPoolSize, 2> bindlessDescriptorPoolSizes {{
            { VK_DESCRIPTOR_TYPE_SAMPLER, maxSamplerCount},
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, maxBindlessResourceCount}
        }};
        VkDescriptorPoolCreateInfo poolCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT, // Allows us to update textures in a bindless array
            // .maxSets = maxBindlessResourceCount * static_cast<uint32_t>(bindlessDescriptorPoolSizes.size()), // ?
            .maxSets = maxBindlessResourceCount + maxSamplerCount, // ? potentially 1 set for each resource
            .poolSizeCount = static_cast<uint32_t>(bindlessDescriptorPoolSizes.size()),
            .pPoolSizes = bindlessDescriptorPoolSizes.data()
        };
        vkCreateDescriptorPool(m_GfxDevice, &poolCreateInfo, nullptr, &m_bindlessDescriptorPool);

        // Build a descriptor set layout
        std::vector<VkDescriptorSetLayoutBinding> bindlessDescriptorSetLayoutBindings;
        uint32_t bindingIndex = 0;
        for(VkDescriptorPoolSize poolSize : bindlessDescriptorPoolSizes)
        {
            VkDescriptorSetLayoutBinding newBinding = {
                .binding = bindingIndex,
                .descriptorType = poolSize.type,
                .descriptorCount = poolSize.descriptorCount,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr
            };
            bindlessDescriptorSetLayoutBindings.push_back(newBinding);
            bindingIndex++;
        }
        // Flags required for bindless stuff
        // We only need a single layout since they are all the same for each frame in flight
        // m_sceneDataDescriptorSetLayouts.push_back(layoutBuilder.buildLayout(m_GfxDevice, VK_SHADER_STAGE_FRAGMENT_BIT));
        const VkDescriptorBindingFlags bindlessFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT
                                                        | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
        std::vector<VkDescriptorBindingFlags> descriptorBindingFlags;
        for(size_t i = 0; i < bindlessDescriptorSetLayoutBindings.size(); i++)
        {
            descriptorBindingFlags.push_back(bindlessFlags);
        }
        descriptorBindingFlags.back() |= VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT; // Permits use of variable array size for a set (with the caveat that only the last binding in the set can be of variable length)
        VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extendedBindingInfo {
            .sType =  VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT,
            .bindingCount = static_cast<uint32_t>(descriptorBindingFlags.size()),
            .pBindingFlags = descriptorBindingFlags.data()
        };
        VkDescriptorSetLayoutCreateInfo bindlessSetLayoutCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = &extendedBindingInfo,
            .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT,
            .bindingCount = static_cast<uint32_t>(bindlessDescriptorSetLayoutBindings.size()),
            .pBindings = bindlessDescriptorSetLayoutBindings.data()
        };
        vkCreateDescriptorSetLayout(m_GfxDevice, &bindlessSetLayoutCreateInfo, nullptr, &m_bindlessDescriptorSetLayout);

        // Allocate the descriptor set
        uint32_t maxBinding = maxBindlessResourceCount;
        VkDescriptorSetVariableDescriptorCountAllocateInfoEXT variableDescriptorCountInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT,
            .descriptorSetCount = 1,
            .pDescriptorCounts = &maxBinding
        };
        VkDescriptorSetAllocateInfo allocateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = &variableDescriptorCountInfo,
            .descriptorPool = m_bindlessDescriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &m_bindlessDescriptorSetLayout
        };

        vkAllocateDescriptorSets(m_GfxDevice, &allocateInfo, &m_bindlessDescriptorSet);
    }

    void BindlessManager::UpdateBindlessTextures() {
        constexpr uint32_t bindlessTextureBinding = 1;

        // TODO: should batch things per frame?

        // Done like this instead of constructing temps in a for loop because of pImageInfo
        std::vector<VkDescriptorImageInfo> textureInfos;
        std::vector<VkWriteDescriptorSet> textureDescriptorWrites;
        textureInfos.resize(m_TextureCache.get_gpu_texture_count());
        textureDescriptorWrites.resize(m_TextureCache.get_gpu_texture_count());

        for (uint32_t i = 0; i < m_TextureCache.get_gpu_texture_count(); i++)
        {
            textureInfos[i].imageView = m_TextureCache.get_texture(i).allocatedImage.imageView;
            textureInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            textureDescriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            textureDescriptorWrites[i].pNext = nullptr;
            textureDescriptorWrites[i].dstSet = m_bindlessDescriptorSet;
            textureDescriptorWrites[i].dstBinding = bindlessTextureBinding;
            textureDescriptorWrites[i].dstArrayElement = i;
            textureDescriptorWrites[i].descriptorCount = 1;
            textureDescriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            textureDescriptorWrites[i].pImageInfo = &textureInfos[i];
            textureDescriptorWrites[i].pBufferInfo = nullptr;
            textureDescriptorWrites[i].pTexelBufferView = nullptr;
        }

        vkUpdateDescriptorSets(m_GfxDevice, static_cast<uint32_t>(textureDescriptorWrites.size()), textureDescriptorWrites.data(), 0, nullptr);
    }

    void BindlessManager::UpdateBindlessSamplers(VkSampler linearSampler, VkSampler shadowSampler) {
        constexpr uint32_t bindlessSamplerBinding = 0;
        VkDescriptorImageInfo linearSamplerInfo = {
            .sampler = linearSampler
        };
        VkDescriptorImageInfo shadowSamplerInfo = {
            .sampler = shadowSampler
        };
        VkWriteDescriptorSet linearSamplerDescriptorWrite = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = m_bindlessDescriptorSet,
            .dstBinding = bindlessSamplerBinding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .pImageInfo = &linearSamplerInfo,
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr
        };
        VkWriteDescriptorSet shadowSamplerDescriptorWrite = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = m_bindlessDescriptorSet,
            .dstBinding = bindlessSamplerBinding,
            .dstArrayElement = 1,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .pImageInfo = &shadowSamplerInfo,
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr
        };
        std::array<VkWriteDescriptorSet, 2> samplerDescriptorWrites = {linearSamplerDescriptorWrite, shadowSamplerDescriptorWrite};

        vkUpdateDescriptorSets(m_GfxDevice, static_cast<uint32_t>(samplerDescriptorWrites.size()), samplerDescriptorWrites.data(), 0, nullptr);
    }
}
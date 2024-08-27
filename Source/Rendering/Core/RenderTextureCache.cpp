#include "RenderTextureCache.h"
#include <Rendering/Core/GfxDevice.h>

namespace MagicRed::Rendering
{
    [[nodiscard]] GPUTextureId RenderTextureCache::add_render_texture(const GfxDevice& gfxDevice, VkFormat format, VkImageCreateInfo imageCreateInfo) {
        // const GPUTextureId textureId = static_cast<uint32_t>(m_gpuTextures.size());
        const GPUTextureId textureId = static_cast<uint32_t>(m_gpuRTTextures.size());

        GPUTexture renderTexture;

        VkExtent3D imageExtent; 
        imageExtent.width = WINDOW_WIDTH;
        imageExtent.height = WINDOW_HEIGHT;
        imageExtent.depth = 1;
        renderTexture.allocatedImage.imageExtent = imageExtent;
        renderTexture.allocatedImage.imageFormat = format;

        create_gpu_only_image(renderTexture.allocatedImage, imageCreateInfo, gfxDevice.m_vmaAllocator);
        VkImageViewCreateInfo imageViewCreateInfo = imageview_create_info(renderTexture.allocatedImage.image, format, {}, VK_IMAGE_ASPECT_COLOR_BIT);
        vkCreateImageView(gfxDevice, &imageViewCreateInfo, nullptr, &renderTexture.allocatedImage.imageView);
        

        // m_gpuTextures.push_back(renderTexture);
        m_gpuRTTextures.push_back(renderTexture);
        return textureId;
    }

    void RenderTextureCache::cleanup(const GfxDevice& gfxDevice) {

        for (auto &texture : m_gpuRTTextures)
        {
            vkDestroyImageView(gfxDevice, texture.allocatedImage.imageView, nullptr);
            vmaDestroyImage(gfxDevice.m_vmaAllocator, texture.allocatedImage.image, texture.allocatedImage.allocation);
        }
    }
}
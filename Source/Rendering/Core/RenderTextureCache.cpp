#include "RenderTextureCache.h"
#include <Rendering/Core/GfxDevice.h>

namespace MagicRed::Rendering
{
    [[nodiscard]] GPUTextureId RenderTextureCache::add_render_texture(const GfxDevice& gfxDevice, VkImageCreateInfo imageCreateInfo) {
        const GPUTextureId textureId = static_cast<uint32_t>(m_gpuRTTextures.size());

        bool isDepthFormat = (imageCreateInfo.format >= 124) && (imageCreateInfo.format <= 130) ? true : false;

        GPUTexture renderTexture;
        renderTexture.allocatedImage.imageExtent = imageCreateInfo.extent;
        renderTexture.allocatedImage.imageFormat = imageCreateInfo.format;

        create_gpu_only_image(renderTexture.allocatedImage, imageCreateInfo, gfxDevice.m_vmaAllocator);
        VkImageViewCreateInfo imageViewCreateInfo = imageview_create_info(
            renderTexture.allocatedImage.image, imageCreateInfo.format, {}, isDepthFormat ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);
        vkCreateImageView(gfxDevice, &imageViewCreateInfo, nullptr, &renderTexture.allocatedImage.imageView);
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
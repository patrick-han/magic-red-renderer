#pragma once
#include <vulkan/vulkan.h>
#include <Rendering/Core/RenderingConfig.h>

namespace MagicRed::Rendering
{
    [[nodiscard]] VkRenderingAttachmentInfoKHR rendering_attachment_info(
        VkImageView imageView,
        VkImageLayout imageLayout,
        const VkClearValue *clearValue
        );

    [[nodiscard]] VkRenderingInfoKHR rendering_info_fullscreen(
        uint32_t colorAttachmentCount,
        VkRenderingAttachmentInfoKHR* pColorAttachments,
        VkRenderingAttachmentInfoKHR* pDepthAttachment
    );
}
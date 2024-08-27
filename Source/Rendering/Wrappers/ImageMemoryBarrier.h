#pragma once
#include <vulkan/vulkan.h>

namespace MagicRed::Rendering
{
    [[nodiscard]] VkImageMemoryBarrier create_image_memory_barrier(
        VkImage image,
        VkAccessFlags srcAccessMask,
        VkAccessFlags dstAccessMask,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT
    );
}
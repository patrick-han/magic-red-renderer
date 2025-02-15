#pragma once

#include <vulkan/vulkan.h>
#include <Common/IdTypes.h>
#include <Common/Math/Matrix4f.h>

namespace MagicRed::Rendering
{
    struct DefaultPushConstants {
        Matrix4f model;
        VkDeviceAddress sceneDataBufferAddress;
        MaterialId materialId;

        static constexpr VkPushConstantRange range() {
            VkPushConstantRange defaultPushConstantRange = {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset = 0,
                .size = sizeof(DefaultPushConstants)
            };
            return defaultPushConstantRange;
        }
    };

    struct ShadowMapPushConstants {
        Matrix4f model;
        VkDeviceAddress sceneDataBufferAddress;

        static constexpr VkPushConstantRange range() {
            VkPushConstantRange shadowMapPushConstantRange = {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = 0,
                .size = sizeof(ShadowMapPushConstants)
            };
            return shadowMapPushConstantRange;
        }
    };
}
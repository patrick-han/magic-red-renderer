#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <Rendering/Light/DirectionalLight.h>


namespace MagicRed::Rendering
{
    struct CPUSceneData
    {
        glm::mat4 view; // 0-64
        glm::mat4 projection; // 64-128
        glm::vec4 cameraWorldPosition; // 128-144
        
        VkDeviceAddress lightBufferAddress; // 144-152
        int numPointLights; // 152-156
        DirectionalLight directionalLight; // 156-172
        int padding; // 172-176
        VkDeviceAddress materialBufferAddress; // 176-184
        glm::mat4 directionalLightViewProjection; // 184-200
    };
}


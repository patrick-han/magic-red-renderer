#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <Rendering/Light/DirectionalLight.h>


namespace MagicRed::Rendering
{
    struct CPUSceneData
    {
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 directionalLightViewProjection;
        
        glm::vec3 cameraWorldPosition;
        int numPointLights;
        VkDeviceAddress lightBufferAddress;
        
        DirectionalLight directionalLight;
        VkDeviceAddress materialBufferAddress;
        
    };
}


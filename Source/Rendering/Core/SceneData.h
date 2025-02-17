#include <vulkan/vulkan.h>
#include <Rendering/Light/DirectionalLight.h>
#include <Common/Math/Matrix4f.h>
#include <Common/Math/Vector4f.h>


namespace MagicRed::Rendering
{
    struct CPUSceneData
    {
        Matrix4f view; // 0-64
        Matrix4f projection; // 64-128
        Vector4f cameraWorldPosition; // 128-144
        
        VkDeviceAddress lightBufferAddress; // 144-152
        int numPointLights; // 152-156
        DirectionalLight directionalLight; // 156-172
        int padding; // 172-176
        VkDeviceAddress materialBufferAddress; // 176-184
        Matrix4f directionalLightViewProjection; // 184-200
    };
}


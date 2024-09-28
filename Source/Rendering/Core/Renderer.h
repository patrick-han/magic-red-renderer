#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

#include <Rendering/Core/GfxDevice.h>
#include <Rendering/Mesh/MeshCache.h>
#include <Rendering/Mesh/RenderMeshComponent.h>
#include <Rendering/Mesh/DefaultPushConstants.h>
#include <Rendering/Pipeline/GraphicsPipeline.h>
#include <Rendering/Texture/TextureCache.h>
#include <Rendering/Core/RenderTextureCache.h>
#include <Rendering/Material/MaterialCache.h>
#include <Rendering/Light/PointLight.h>
#include <Rendering/Light/DirectionalLight.h>
#include <Rendering/Buffer/Buffer.h>
#include <Rendering/Core/SceneData.h>
#include <Rendering/Core/RenderingConfig.h>

#include <Rendering/RenderStages/ShadowMapStage.h>
#include <Rendering/RenderStages/GBufferStage.h>
#include <Rendering/RenderStages/BlinnPhongLightingStage.h>

#include <Resource/GUID.h>


namespace MagicRed::Resource
{
    class ResourceManager;
    class CPUModelLoader;
}

namespace MagicRed::Rendering
{
    class SDL_window;

    class Renderer {
    public:
        Renderer();
        ~Renderer();
        void Startup();
        void run();
        void Shutdown();

        // Mesh Cache
        inline GPUMeshId UploadMesh(const CPUMesh& cpuMesh, const MaterialId meshMaterialId) {
            return m_MeshCache.add_mesh(m_GfxDevice, cpuMesh, meshMaterialId);
        }

        // Texture Cache
        [[nodiscard]] inline GPUTextureId UploadTexture(const TextureLoadingData& texLoadingData, const MagicRed::Resource::GUID guid) {
            return m_TextureCache.upload_texture(m_GfxDevice, texLoadingData, guid);
        }

        [[nodiscard]] inline GPUTextureId GetGPUTextureIdByGuid(const MagicRed::Resource::GUID guid) const {
            return m_TextureCache.get_gpu_texture_id_by_guid(guid);
        }


        // GPUMaterial Cache
        MaterialId AddMaterial(const GPUMaterial& material);

        const MagicRed::Resource::GUID m_defaultTexturePlaceholderGuid;
        GPUTextureId m_defaultTexturePlaceholderId{NULL_GPU_TEXTURE_ID};
        const MagicRed::Resource::GUID m_missingDiffuseTextureGuid;
        GPUTextureId m_missingDiffuseTextureId{NULL_GPU_TEXTURE_ID};
        MaterialId m_defaultMaterialId{NULL_MATERIAL_ID};

        friend class MagicRed::Resource::ResourceManager;
    private:
        SDL_Window *m_window;
        GfxDevice m_GfxDevice;

        MeshCache m_MeshCache;
        TextureCache m_TextureCache;
        MaterialCache m_MaterialCache;
        RenderTextureCache m_RenderTextureCache;


        VkDescriptorPool m_imguiPool;
        uint32_t m_currentFrame = 0;

        std::vector<RenderMeshComponent> m_sceneRenderMeshComponents;

        // Lights
        std::vector<PointLight> m_CPUPointLights;
        std::array<AllocatedBuffer, MAX_FRAMES_IN_FLIGHT> m_GPUPointLightsBuffers;
        bool m_pointLightsExist = false;
        DirectionalLight m_directionalLight;

        // MaterialData
        AllocatedBuffer m_materialDataBuffer;

        // SceneData
        CPUSceneData m_CPUSceneData;
        std::array<AllocatedBuffer, MAX_FRAMES_IN_FLIGHT> m_GPUSceneDataBuffers;

        // Descriptors
        VkDescriptorPool m_bindlessPool;
        VkDescriptorSetLayout m_bindlessDescriptorSetLayout;
        VkDescriptorSet m_bindlessDescriptorSet;

        VkDescriptorPool m_globalDescriptorPool;

        // Samplers
        VkSampler m_linearSampler;
        VkSampler m_nearestSampler;

        // Imgui
        bool m_bShowRenderingMenu = true;
        bool m_bInteractableUI = false;

        // RTs TODO:
        GPUTextureId m_directionalShadowMapRTId{NULL_GPU_TEXTURE_ID};

        GPUTextureId m_albedoRTId{NULL_GPU_TEXTURE_ID};
        GPUTextureId m_worldNormalsRTId{NULL_GPU_TEXTURE_ID};
        GPUTextureId m_metallicRoughnessRTId{NULL_GPU_TEXTURE_ID};

        GPUTextureId m_lightingRTId{NULL_GPU_TEXTURE_ID};
        
        // std::vector<std::unique_ptr<StageBase>> m_pRenderStages;
        std::unique_ptr<ShadowMapStage> m_pShadowMapStage;
        std::unique_ptr<GBufferStage> m_pGbufferStage;
        std::unique_ptr<BlinnPhongLightingStage> m_pLightingStage;

        float rx{1.0f};
        float ry{0.0f};
        float rz{0.0f};
        float rm{ 3.14f * 3.0f / 2.0f };

        void initWindow();
        void init_graphics();

        
        void init_lights();
        void create_samplers();
        void init_bindless_descriptors();
        void init_assets();
        void update_material_data();
        void init_scene_data();

        void init_global_descriptor_pool();

        void init_render_textures();
        void init_render_stages();

        void update_bindless_texture_descriptors();
        
        void init_imgui();
        
        void draw_imgui(VkImageView targetImageView);
        void update_lights(uint32_t frameInFlightIndex);
        void update_scene_data(uint32_t frameInFlightIndex);
        void drawFrame();
        void mainLoop();
        void cleanup();
    };
}

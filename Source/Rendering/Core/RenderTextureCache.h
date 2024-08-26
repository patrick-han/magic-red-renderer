#pragma once
#include <Rendering/Texture/GPUTexture.h>
#include <Common/IdTypes.h>
#include <vector>

namespace MagicRed::Rendering
{
    class GfxDevice;
    class RenderTextureCache
    {
    public:
        RenderTextureCache() = default;
        ~RenderTextureCache() = default;
        RenderTextureCache(const RenderTextureCache&) = delete;
        RenderTextureCache& operator=(const RenderTextureCache&) = delete;
        RenderTextureCache(RenderTextureCache&&) = delete;
        RenderTextureCache& operator=(RenderTextureCache&&) = delete;

        [[nodiscard]] GPUTextureId add_render_texture(const GfxDevice& gfxDevice, VkFormat format, VkImageCreateInfo imageCreateInfo);

        [[nodiscard]] inline const GPUTexture& get_render_texture(GPUTextureId id) const
        {
            return m_gpuRTTextures[id];
        }

        void cleanup(const GfxDevice& gfxDevice);

    private:
        std::vector<GPUTexture> m_gpuRTTextures;
    };
}

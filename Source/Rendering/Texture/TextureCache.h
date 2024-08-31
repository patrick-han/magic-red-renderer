#pragma once
#include <Rendering/Texture/GPUTexture.h>
#include <Rendering/Texture/TextureData.h>
#include <Common/IdTypes.h>
#include <vector>
#include <unordered_map>
#include <string>

#include <Resource/GUID.h>

namespace MagicRed::Rendering
{
    class GfxDevice;
    class TextureCache
    {
    public:
        TextureCache() = default;
        ~TextureCache() = default;
        TextureCache(const TextureCache&) = delete;
        TextureCache& operator=(const TextureCache&) = delete;
        TextureCache(TextureCache&&) = delete;
        TextureCache& operator=(TextureCache&&) = delete;

        [[nodiscard]] GPUTextureId upload_texture(const GfxDevice& gfxDevice, const TextureLoadingData& texLoadingData, MagicRed::Resource::GUID guid);

        [[nodiscard]] inline const GPUTexture& get_texture(GPUTextureId id) const {
            return m_gpuResidentTextures[id];
        }

        [[nodiscard]] inline GPUTextureId get_gpu_texture_id_by_guid(MagicRed::Resource::GUID guid) const {
            return m_texturesUploadedAlready.at(guid);
        }

        [[nodiscard]] inline uint32_t get_gpu_texture_count() const {
            return static_cast<uint32_t>(m_gpuResidentTextures.size());
        }

        void cleanup(const GfxDevice& gfxDevice);

    private:
        void upload_texture(const GfxDevice& gfxDevice, const TextureLoadingData& texLoadingData);
        [[nodiscard]] inline bool is_texture_uploaded_already(MagicRed::Resource::GUID guid) const {
            return m_texturesUploadedAlready.count(guid) > 0 ? true : false;
        }
        std::vector<GPUTexture> m_gpuResidentTextures;
        std::unordered_map<MagicRed::Resource::GUID, GPUTextureId> m_texturesUploadedAlready; // std::string (or string_view?) required since doing const char* is comparing different pointers each time
        // TODO: It should really not using std:string as a key, since there is O(N) cost on the string length for both hashing and comparison...
    };
}

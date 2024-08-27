#pragma once
#include <Rendering/Texture/GPUTexture.h>
#include <Rendering/Texture/TextureData.h>
#include <Common/IdTypes.h>
#include <vector>
#include <unordered_map>
#include <string>

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

        [[nodiscard]] GPUTextureId add_texture(const GfxDevice& gfxDevice, const TextureLoadingData& texLoadingData, const std::string& textureName);

        [[nodiscard]] inline const GPUTexture& get_texture(GPUTextureId id) const {
            return m_gpuTextures[id];
        }

        [[nodiscard]] inline GPUTextureId get_texture_id(const std::string& textureName) const {
            return m_texturesLoadedAlready.at(textureName);
        }

        [[nodiscard]] inline uint32_t get_texture_count() const {
            return static_cast<uint32_t>(m_gpuTextures.size());
        }

        [[nodiscard]] inline bool is_texture_loaded_already(const std::string& textureName) const {
            return m_texturesLoadedAlready.count(textureName) > 0 ? true : false;
        }

        void cleanup(const GfxDevice& gfxDevice);

    private:
        void upload_texture(const GfxDevice& gfxDevice, const TextureLoadingData& texLoadingData);
        std::vector<GPUTexture> m_gpuTextures;
        std::unordered_map<std::string, GPUTextureId> m_texturesLoadedAlready; // std::string (or string_view?) required since doing const char* is comparing different pointers each time
        // TODO: It should really not using std:string as a key, since there is O(N) cost on the string length for both hashing and comparison...
    };
}

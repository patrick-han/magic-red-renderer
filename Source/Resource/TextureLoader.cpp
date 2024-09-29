#include <Resource/TextureLoader.h>
#include <Resource/GUID.h>
#include <External/stb_image.h>
#include <Rendering/Texture/TextureData.h>
#include <Rendering/Core/Renderer.h>
#include <assimp/texture.h>

namespace MagicRed::Resource
{
    TextureLoader::TextureLoader(
        MagicRed::Rendering::Renderer* _pRenderer
        , std::unordered_map<std::filesystem::path, GUID>& _textureFileToGuidMapRef
    )
    : m_pRenderer(_pRenderer)
    , m_textureFileToGuidMapRef(_textureFileToGuidMapRef)
    {

    }

    [[nodiscard]] GPUTextureId TextureLoader::LoadTextureFromFile(const std::filesystem::path& texturePath, const aiTexture* embeddedTexture)
    {
        GPUTextureId gpuTextureId;

        // Now, we actually load the texture file if it hasn't already been loaded and uploaded to the GPU
        if (m_textureFileToGuidMapRef.count(texturePath) == 0)
        {
            
            int width, height, numberComponents;
            stbi_uc* data;
            if (embeddedTexture == nullptr) {
                data = stbi_load(texturePath.string().c_str(), &width, &height, &numberComponents, STBI_rgb_alpha); // TODO: request 4 channels from all images
            } else {
                data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(embeddedTexture->pcData), embeddedTexture->mWidth, &width, &height, &numberComponents, STBI_rgb_alpha);
            }
            
            MagicRed::Rendering::TextureLoadingData textureLoadingData = {
                .data = data,
                .texSize = {width, height, 4} // TODO: force all images to have 4 channels...ignoring numberComponents for now
            };

            GUID newTextureGuid = GUID();
            m_textureFileToGuidMapRef.insert({texturePath, newTextureGuid});
            gpuTextureId = m_pRenderer->UploadTexture(textureLoadingData, newTextureGuid);
            stbi_image_free(data);
        }
        else // Otherwise retrieve the existing guid...
        {
            gpuTextureId = m_pRenderer->GetGPUTextureIdByGuid(m_textureFileToGuidMapRef[texturePath]);
        }
        return gpuTextureId;
    }
}

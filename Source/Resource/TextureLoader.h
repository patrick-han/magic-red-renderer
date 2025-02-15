#pragma once

#include <Common/IdTypes.h>
#include <filesystem>
#include <unordered_map>
#include <Resource/GUID.h>

struct aiTexture;

namespace MagicRed::Rendering
{
    class Renderer;
}

namespace MagicRed::Resource
{
    class TextureLoader
    {
    public:
        TextureLoader(
            MagicRed::Rendering::Renderer* _pRenderer
            , std::unordered_map<std::filesystem::path, GUID>& _textureFileToGuidMapRef
        );
        [[nodiscard]] GPUTextureId LoadTextureFromFile(const std::filesystem::path& texturePath, const aiTexture* embeddedTexture = nullptr);
    private:
        MagicRed::Rendering::Renderer* m_pRenderer;
        std::unordered_map<std::filesystem::path, GUID>& m_fileToGuidMap;
    };
}

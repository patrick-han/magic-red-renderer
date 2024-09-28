#pragma once

#include <Common/IdTypes.h>
#include <filesystem>
#include <unordered_map>
#include <Resource/GUID.h>

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
        [[nodiscard]] GPUTextureId LoadTextureFromFile(const std::filesystem::path& texturePath);
    private:
        MagicRed::Rendering::Renderer* m_pRenderer;
        std::unordered_map<std::filesystem::path, GUID>& m_textureFileToGuidMapRef;
    };
}

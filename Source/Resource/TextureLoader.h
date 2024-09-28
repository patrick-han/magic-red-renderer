#pragma once

#include <Common/IdTypes.h>
#include <filesystem>

namespace MagicRed::Resource
{
    class TextureLoader
    {
    public:
        TextureLoader();
        GPUTextureId LoadTextureFromFile(const std::filesystem::path& texturePath);
    };
}

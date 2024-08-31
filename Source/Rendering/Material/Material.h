#pragma once

#include <Common/IdTypes.h>
#include <Rendering/Texture/TextureData.h>
#include <Resource/GUID.h>

namespace MagicRed::Rendering
{
    // This point to Ids in a TextureCache
    struct GPUMaterial {
        // TODO:
        // "baseColorFactor": [ 1.000, 0.766, 0.336, 1.0 ],
        // "metallicFactor": 1.0,
        // "roughnessFactor": 0.0
        GPUTextureId diffuseTextureId{NULL_GPU_TEXTURE_ID};
        GPUTextureId normalTextureId{NULL_GPU_TEXTURE_ID};
        GPUTextureId metallicRoughnessTextureId{NULL_GPU_TEXTURE_ID};
        GPUTextureId emissiveTextureId{NULL_GPU_TEXTURE_ID};
    };

    struct MaterialAsset {
        MagicRed::Resource::GUID diffuseTextureGuid;
        MagicRed::Resource::GUID normalTextureGuid;
        MagicRed::Resource::GUID metallicRoughnessTextureGuid;
        MagicRed::Resource::GUID emissiveTextureGuid;
    };
}
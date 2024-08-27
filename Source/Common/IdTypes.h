#pragma once
#include <limits>


#include <cstdint>
#include <limits>


namespace MagicRed
{
    using GPUMeshId = std::size_t;
    static const auto NULL_GPU_MESH_ID = std::numeric_limits<std::size_t>::max();

    using GPUTextureId = std::uint32_t;
    static const auto NULL_GPU_TEXTURE_ID = std::numeric_limits<std::uint32_t>::max();

    using MaterialId = std::uint32_t;
    static const auto NULL_MATERIAL_ID = std::numeric_limits<std::uint32_t>::max();
}
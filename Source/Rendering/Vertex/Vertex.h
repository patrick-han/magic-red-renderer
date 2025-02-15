#pragma once
#include <Common/Math/Vector3f.h>
#include <Common/Math/Vector4f.h>
namespace MagicRed::Rendering
{
    // https://github.com/eliasdaler/edbr/blob/master/edbr/include/edbr/Graphics/CPUMesh.h
    struct Vertex {
        Vector3f position;
        float uv_x{};
        Vector3f normal;
        float uv_y{};
        Vector4f tangent;
        Vector4f color;
    };
}

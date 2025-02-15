#pragma once

#include <Common/Math/Vector3f.h>

namespace MagicRed::Rendering
{
    struct DirectionalLight {
        DirectionalLight();
        DirectionalLight(const Vector3f& _direction, float _power);
        Vector3f direction; // Defined as pointing _away_ from the light
        float power;
    };
    }
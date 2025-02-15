#pragma once
#include <Common/Math/Vector3f.h>

namespace MagicRed::Rendering
{
    struct PointLight {
        PointLight(
              const Vector3f& _worldSpacePosition
            , const Vector3f& _color
            , float constantAtten
            , float linearAtten
            , float quadraticAtten
        );

        Vector3f worldSpacePosition;      
        Vector3f ambient;      
        Vector3f diffuse;
        Vector3f specular;
        float constantAttenuation;
        float linearAttenuation;
        float quadraticAttenuation;
    };
}
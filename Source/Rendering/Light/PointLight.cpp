#include "PointLight.h"
#include <utility>

namespace MagicRed::Rendering
{
    PointLight::PointLight(const Vector3f& _worldSpacePosition, const Vector3f& _color, float constantAtten, float linearAtten, float quadraticAtten)
        : worldSpacePosition(_worldSpacePosition)
        , ambient(_color)
        , diffuse(_color)
        , specular(_color)
        , constantAttenuation(constantAtten)
        , linearAttenuation(linearAtten)
        , quadraticAttenuation(quadraticAtten)
    {}
}

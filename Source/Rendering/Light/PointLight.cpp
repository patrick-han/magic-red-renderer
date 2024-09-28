#include "PointLight.h"
#include <utility>

namespace MagicRed::Rendering
{
    PointLight::PointLight(glm::vec3 _worldSpacePosition, glm::vec3 _color, float constantAtten, float linearAtten, float quadraticAtten)
        : worldSpacePosition(_worldSpacePosition)
        , constantAttenuation(constantAtten)
        , ambient(_color)
        , linearAttenuation(linearAtten)
        , diffuse(_color)
        , quadraticAttenuation(quadraticAtten)
        , specular(_color)
        // , padding(0.0f) // Initialize padding to 0
    {}
}

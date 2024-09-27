#include "PointLight.h"
#include <utility>

namespace MagicRed::Rendering
{
    PointLight::PointLight(glm::vec3 _worldSpacePosition, glm::vec3 _color, float constantAtten, float linearAtten, float quadraticAtten)
        : worldSpacePosition(_worldSpacePosition)
        , ambient(_color)
        , diffuse(_color)
        , specular(_color)
        , attenuationTerms{constantAtten, linearAtten, quadraticAtten}
    {}
}

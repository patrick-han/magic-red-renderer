#pragma once

#include <glm/vec3.hpp>


namespace MagicRed::Rendering
{
    struct PointLight {
        PointLight(
            glm::vec3 _worldSpacePosition
            , glm::vec3 _color
            , float constantAtten
            , float linearAtten
            , float quadraticAtten
        );

        glm::vec3 worldSpacePosition;      
        glm::vec3 ambient;      
        glm::vec3 diffuse;
        glm::vec3 specular;
        float constantAttenuation;
        float linearAttenuation;
        float quadraticAttenuation;
    };
}
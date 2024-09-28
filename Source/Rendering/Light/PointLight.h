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
        float constantAttenuation;
        glm::vec3 ambient;
        float linearAttenuation;
        glm::vec3 diffuse;
        float quadraticAttenuation;
        glm::vec3 specular;
        // float padding; // Add padding to ensure 16-byte alignment
    };
}
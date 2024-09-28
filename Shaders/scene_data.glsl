#ifndef SCENE_DATA_GLSL
#define SCENE_DATA_GLSL

#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require

struct PointLight {
    vec3 worldSpacePosition;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

struct DirectionalLight {
    vec3 direction;
    float power;
};

layout (buffer_reference, scalar) readonly buffer PointLightsDataBuffer {
    PointLight data[];
};

#include "material.glsl"

layout (buffer_reference, scalar) readonly buffer SceneDataBuffer {
    // camera
    mat4 view;               
    mat4 projection;         
    vec3 cameraWorldPosition;

    PointLightsDataBuffer pointLights;
    int numPointLights;
    DirectionalLight directionalLight;

    MaterialDataBuffer materials;
    vec4 directionalLightViewProjection;
};

#endif // SCENE_DATA_GLSL
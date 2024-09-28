#version 450

#extension GL_GOOGLE_include_directive : require

#include "scene_data.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in float uv_x;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in float uv_y;
layout (location = 4) in vec3 vTangent;
layout (location = 5) in vec4 vColor;

layout (push_constant) uniform PushConstants
{
    mat4 modelMatrix;
    SceneDataBuffer sceneData;
} pushConstants;

void main() {
    gl_Position = pushConstants.sceneData.directionalLightViewProjection * pushConstants.modelMatrix * vec4(vPosition, 1.0);
}
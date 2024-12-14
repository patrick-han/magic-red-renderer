#version 450

#extension GL_GOOGLE_include_directive : require

#include "scene_data.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in float uv_x;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in float uv_y;
layout (location = 4) in vec4 vTangent;
layout (location = 5) in vec4 vColor;

layout (location = 0) out vec3 fragWorldPos;
layout (location = 1) out vec3 fragWorldSurfaceNormal;
layout (location = 2) out vec2 textureCoords;
layout (location = 3) out vec4 fragColor;
layout (location = 4) out mat3 outTBN;

#include "mesh_push_constants.glsl"

void main() {
    fragWorldPos = vec3(pushConstants.modelMatrix * vec4(vPosition, 1.0));
    fragWorldSurfaceNormal = mat3(transpose(inverse(pushConstants.modelMatrix))) * vNormal;
    textureCoords = vec2(uv_x, uv_y);
    fragColor = vColor;

    vec4 vBitangent = vec4(cross(vNormal, vTangent.xyz), 0.0);
    vec3 T = normalize(vec3(pushConstants.modelMatrix * vTangent));
    vec3 B = normalize(vec3(pushConstants.modelMatrix * vBitangent));
    vec3 N = normalize(vec3(pushConstants.modelMatrix * vec4(vNormal, 0.0)));
    mat3 TBN = mat3(T, B, N);
    outTBN = TBN;

    gl_Position = pushConstants.sceneData.projection * pushConstants.sceneData.view * vec4(fragWorldPos, 1.0);
}
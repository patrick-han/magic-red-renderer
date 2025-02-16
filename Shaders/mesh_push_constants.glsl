#ifndef MESH_PUSH_CONSTANTS_GLSL
#define MESH_PUSH_CONSTANTS_GLSL

#include "scene_data.glsl"

// Push constants block
layout (push_constant) uniform PushConstants
{
    layout(row_major) mat4 modelMatrix;
    SceneDataBuffer sceneData;
    uint materialId;
} pushConstants;


#endif // MESH_PUSH_CONSTANTS_GLSL
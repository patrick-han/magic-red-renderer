#version 450

// #extension GL_GOOGLE_include_directive : require
// #extension GL_EXT_nonuniform_qualifier : require
// #include "scene_data.glsl"
// #include "mesh_push_constants.glsl"


layout(location = 0) out vec4 outColor;

void main() {
    float depth = gl_FragCoord.z;
    gl_FragDepth = depth;
    outColor = vec4(depth, depth, depth, 1.0);
}
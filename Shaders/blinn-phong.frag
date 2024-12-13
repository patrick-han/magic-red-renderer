#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_samplerless_texture_functions : require
#include "scene_data.glsl"

layout(location = 0) in vec2 textureCoords;
layout(location = 0) out vec4 outColor;

#include "mesh_push_constants.glsl"

// TODO subpasses w/ VK_KHR_dynamic_rendering_local_read
// layout (set = 0, binding = 0) uniform sampler linearSampler;
#define LINEAR_SAMPLER 0
#define SHADOW_SAMPLER 1
layout (set = 0, binding = 0) uniform sampler samplers[];
layout (set = 0, binding = 1) uniform texture2D textures[];

vec4 sampleTextureLinear(texture2D tex, vec2 texCoords) {
    return texture(sampler2D(tex, samplers[LINEAR_SAMPLER]), texCoords);
}

vec4 sampleTextureShadow(texture2D tex, vec2 texCoords) {
    return texture(sampler2D(tex, samplers[SHADOW_SAMPLER]), texCoords);
}


layout (set = 1, binding = 0) uniform texture2D albedoBuffer;
layout (set = 1, binding = 1) uniform texture2D normalsBuffer;
layout (set = 1, binding = 2) uniform texture2D metallicRoughnessBuffer;
layout (set = 1, binding = 3) uniform texture2D depthBuffer; // For reconstructing world space positions

layout (set = 1, binding = 4) uniform texture2D directionalLightShadowMap;

float calculateShadow(vec3 norm, vec3 fragWorldPos) {
    vec4 fragDirLightSpacePos = pushConstants.sceneData.directionalLightViewProjection * vec4(fragWorldPos, 1.0);
    vec3 shadowSamplePos = fragDirLightSpacePos.xyz / fragDirLightSpacePos.w;

    // Fix oversampling outside the far plane of the light orthographic frustum
    if (shadowSamplePos.z > 1.0) {
        return 0.0;
    }

    shadowSamplePos.xy *= 0.5;
    shadowSamplePos.xy += 0.5; // UV

    float inShadow = 0.0;
    vec3 lightDir = normalize(pushConstants.sceneData.directionalLight.direction);
    float bias = max(0.005, 0.05 * (1.0 - dot(norm, lightDir))); // Goes from 0.005 to 0.05 as the angle between the light and the normal increases
    float currentDepth = fragDirLightSpacePos.z;

    // PCF: Simple average over a 3x3 neighborhood
    vec2 texelSize = vec2(1.0) / textureSize(directionalLightShadowMap, 0);
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            vec2 offset = vec2(x,y) * texelSize;
            float shadowMapDepth = sampleTextureShadow(directionalLightShadowMap, shadowSamplePos.xy + offset).r;
            if (shadowMapDepth < (currentDepth - bias)) {
                inShadow += 1.0;
            }
        }
    }

    inShadow /= 9.0;
    return inShadow;
}

vec3 calculateDirectionalLightContribution(vec3 diffuseTexColor, vec2 metallicRoughnessColor, vec3 sampledNormal, vec3 fragWorldPos)
{
    vec3 norm = normalize(sampledNormal);
    vec3 lightColor = vec3(pushConstants.sceneData.directionalLight.power); // TODO: Directional light color?
    float inShadow = calculateShadow(norm, fragWorldPos);

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = diffuseTexColor * ambientStrength * lightColor;

    // Diffuse
    vec3 fragToLightDir = normalize(pushConstants.sceneData.directionalLight.direction);
    float difference = max(dot(fragToLightDir, norm), 0.0);
    vec3 diffuse = diffuseTexColor * difference * lightColor;


    // Specular (Blinn)
    float specularStrength = 0.5;
    vec3 viewDir = normalize(pushConstants.sceneData.cameraWorldPosition.xyz - fragWorldPos);
    vec3 halfwayDir = normalize(fragToLightDir + viewDir);
    float specularDifference = pow(max(dot(norm, halfwayDir), 0.0), 32);
    vec3 specular = specularStrength * specularDifference * lightColor;

    vec3 result = (ambient + diffuse + specular) * (1.0 - inShadow);
    return result;
}

vec3 calculatePointLightsContribution(int pointLightIndex, vec3 diffuseTexColor, vec2 metallicRoughnessColor, vec3 sampledNormal, vec3 fragWorldPos)
{
    vec3 lightAmbient = pushConstants.sceneData.pointLights.data[pointLightIndex].ambient;
    vec3 lightDiffuse = pushConstants.sceneData.pointLights.data[pointLightIndex].diffuse;
    vec3 lightSpecular = pushConstants.sceneData.pointLights.data[pointLightIndex].specular;

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = diffuseTexColor * ambientStrength * lightAmbient;

    // Diffuse
    vec3 fragToLight = pushConstants.sceneData.pointLights.data[pointLightIndex].worldSpacePosition - fragWorldPos;
    vec3 fragToLightDir = normalize(fragToLight);
    vec3 norm = normalize(sampledNormal);
    float difference = max(dot(fragToLightDir, norm), 0.0);
    vec3 diffuse = diffuseTexColor * difference * lightDiffuse;


    // Specular (Blinn)
    float specularStrength = 0.5;
    vec3 viewDir = normalize(pushConstants.sceneData.cameraWorldPosition.xyz - fragWorldPos);
    vec3 halfwayDir = normalize(fragToLightDir + viewDir);
    float specularDifference = pow(max(dot(norm, halfwayDir), 0.0), 32);
    vec3 specular = specularStrength * specularDifference * lightSpecular;

    float fragToLightDist = length(fragToLight);

    float attenuation = 1.0 / (
        pushConstants.sceneData.pointLights.data[pointLightIndex].constantAttenuation + 
        pushConstants.sceneData.pointLights.data[pointLightIndex].linearAttenuation * fragToLightDist + 
        pushConstants.sceneData.pointLights.data[pointLightIndex].quadraticAttenuation * fragToLightDist * fragToLightDist
    );

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    vec3 result = (ambient + diffuse + specular);
    return result;
}


void main() {
    // Sample GBuffer
    vec3 sampledColor = texelFetch(sampler2D(albedoBuffer, samplers[LINEAR_SAMPLER]), ivec2(gl_FragCoord.xy), 0).rgb;
    vec3 sampledNormal = normalize(texelFetch(sampler2D(normalsBuffer, samplers[LINEAR_SAMPLER]), ivec2(gl_FragCoord.xy), 0).rgb * 2.0 - 1.0);
    vec2 sampledMetallicRoughness = texelFetch(sampler2D(metallicRoughnessBuffer, samplers[LINEAR_SAMPLER]), ivec2(gl_FragCoord.xy), 0).rg;
    float sampledDepth = texelFetch(depthBuffer, ivec2(gl_FragCoord.xy), 0).r;
    // x,y are [0, 1] and so is depth-z [0, 1]
    // sampledDepth = sampledDepth * 2.0 - 1.0; // [-1, 1] // In Vulkan, NDC is [0, 1] in z, unlike OpenGL which expects [-1, 1]
    vec4 reconstructedDepth = inverse(pushConstants.sceneData.view) * inverse(pushConstants.sceneData.projection) * vec4(textureCoords * 2.0 - 1.0, sampledDepth, 1.0); // TODO: don't invert per invocation
    vec3 fragWorldPos =  reconstructedDepth.xyz / reconstructedDepth.w;

    

    vec3 result = vec3(0.0);

    result += calculateDirectionalLightContribution(sampledColor, sampledMetallicRoughness, sampledNormal, fragWorldPos.xyz);

    for (int i = 0; i < pushConstants.sceneData.numPointLights; i++)
    {
        result += calculatePointLightsContribution(i, sampledColor, sampledMetallicRoughness, sampledNormal, fragWorldPos.xyz);
    }

    outColor = vec4(result, 1.0);
    // outColor = vec4(fragWorldPos.xyz, 1.0);
    // outColor = vec4(sampledDepth, 0.0,0.0,1.0);
}
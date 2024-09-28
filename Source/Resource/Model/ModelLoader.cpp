#include <Common/Compiler/DisableWarnings.h>
PUSH_MSVC_WARNINGS
DISABLE_MSVC_WARNING(4267) // conversion from 'size_t' to 'uint32_t', possible loss of data
DISABLE_MSVC_WARNING(4201) // nonstandard extension used : nameless struct / union (glm library)
PUSH_CLANG_WARNINGS
DISABLE_CLANG_WARNING("-Wmissing-field-initializers")
DISABLE_CLANG_WARNING("-Wshorten-64-to-32")
#include <Common/Compiler/Unused.h>

#include "ModelLoader.h"
#include <Rendering/Texture/TextureCache.h>
#include <Rendering/Material/MaterialCache.h>
#include <Rendering/Material/Material.h>
#include <Rendering/Core/Renderer.h>
#include <vulkan/vulkan.h>
#include <Common/Log.h>
#include <span>

// Define these only in *one* .cpp file.
#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <External/stb_image.h>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <Resource/GUID.h>


namespace MagicRed::Resource
{
    std::string GetTextureNameFromAssimpType(aiMaterial* material, aiTextureType textureType) {
        aiString str;
        material->GetTexture(textureType, 0, &str);
        const std::string textureName(str.C_Str());
        return textureName;
    }

    void CPUModelLoader::load_embedded_texture_data(const aiMaterial* material, const aiScene* scene, aiTextureType textureType, MagicRed::Rendering::GPUMaterial& meshMaterial)
    {
        aiString embeddedTextureFile;
        material->GetTexture(textureType, 0, &embeddedTextureFile);
        const aiTexture* texture = scene->GetEmbeddedTexture(embeddedTextureFile.C_Str());
        std::string textureName = m_filePath.filename().replace_extension().string();

        GPUTextureId* meshMaterialTextureIdToSet = nullptr;

        switch(textureType)
        {
            case aiTextureType_BASE_COLOR:
                textureName += std::string("_diffuse_tex");
                meshMaterialTextureIdToSet = &meshMaterial.diffuseTextureId;
                break;
            case aiTextureType_METALNESS:
                textureName += std::string("_metallic_roughness_tex");
                meshMaterialTextureIdToSet = &meshMaterial.metallicRoughnessTextureId;
                break;
            case aiTextureType_NORMALS:
                textureName += std::string("_normals_tex");
                meshMaterialTextureIdToSet = &meshMaterial.normalTextureId;
                break;
            case aiTextureType_EMISSIVE:
                textureName += std::string("_emissive_tex");
                meshMaterialTextureIdToSet = &meshMaterial.emissiveTextureId;
                break;
            default:
                MRCERR("Tried to load non-standard aiTextureType!");
                exit(1);
        }
        

        if (m_textureFileToGuidMapRef.count(textureName) == 0)
        {
            // New unique texture
            int width, height, numberComponents;
            stbi_uc* data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &numberComponents, STBI_rgb_alpha);
            if (!data)
            {
                MRCERR("Failed to load embedded texture from memory!");
                exit(1);
            }

            MagicRed::Rendering::TextureLoadingData textureLoadingData = {
                .data = data,
                .texSize = {width, height, 4} // TODO: force all images to have 4 channels...ignoring numberComponents for now
            };
            
            // TODO: should embedded textures have guids?
            GUID newTextureGuid = GUID();
            m_textureFileToGuidMapRef.insert({textureName, newTextureGuid});
            *meshMaterialTextureIdToSet = m_pRenderer->UploadTexture(textureLoadingData, newTextureGuid);
            stbi_image_free(textureLoadingData.data);
        }
        else
        {
            GUID textureGuid  = m_textureFileToGuidMapRef[textureName];
            *meshMaterialTextureIdToSet = m_pRenderer->GetGPUTextureIdByGuid(textureGuid);
        }
    }

    void CPUModelLoader::process_mesh(MagicRed::Rendering::CPUMesh& cpuMesh, MaterialId& meshMaterialId, aiMesh *mesh, const aiScene *scene, const glm::mat4x4& transformMatrix)
    {
        cpuMesh.m_transform = transformMatrix;
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            MagicRed::Rendering::Vertex vertex;

            glm::vec4 worldSpaceVertex = transformMatrix * glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
            vertex.position = glm::vec3(worldSpaceVertex.x, worldSpaceVertex.y, worldSpaceVertex.z);
            // vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            if (mesh->HasTextureCoords(0))
            {
                vertex.uv_x = mesh->mTextureCoords[0][i].x;
                vertex.uv_y = mesh->mTextureCoords[0][i].y;
            }

            // TODO:
            // vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            cpuMesh.m_vertices.emplace_back(vertex);
        }
        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
            {
                cpuMesh.m_indices.push_back(face.mIndices[j]);
            } 
        }
        if(mesh->mMaterialIndex >= 0)
        {
            if (m_sceneMaterialsAlreadyLoaded.count(mesh->mMaterialIndex) > 0)
            {
                meshMaterialId = m_sceneMaterialsAlreadyLoaded.at(mesh->mMaterialIndex);
            }
            else
            {
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
                MagicRed::Rendering::GPUMaterial meshMaterial;

                unsigned int materialDiffuseCount = material->GetTextureCount(aiTextureType_BASE_COLOR);
                unsigned int materialMetallicRoughnessCount = material->GetTextureCount(aiTextureType_METALNESS);
                unsigned int materialNormalCount = material->GetTextureCount(aiTextureType_NORMALS);
                unsigned int materialEmissiveCount = material->GetTextureCount(aiTextureType_EMISSIVE);

                aiColor4D aiColor;
                if (material->Get(AI_MATKEY_BASE_COLOR, aiColor) == AI_SUCCESS)
                {
                    for (MagicRed::Rendering::Vertex& vertex : cpuMesh.m_vertices)
                    {
                        vertex.color = glm::vec4(aiColor.r, aiColor.g, aiColor.b, aiColor.a);
                    }
                }
                else
                {
                    for (MagicRed::Rendering::Vertex& vertex : cpuMesh.m_vertices)
                    {
                        vertex.color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f); // Magenta fallback
                    }
                }
                

                // {
                //     aiColor4D baseColorFactor;
                //     if(material->Get(AI_MATKEY_BASE_COLOR, baseColorFactor) == aiReturn_SUCCESS)
                //     {
                //         meshMaterial.baseColorFactor = glm::vec4(baseColorFactor.r, baseColorFactor.g, baseColorFactor.b, baseColorFactor.a);
                //     }
                // }
                // {
                //     float metallicFactor;
                //     if(material->Get(AI_MATKEY_METALLIC_FACTOR, metallicFactor) == aiReturn_SUCCESS)
                //     {
                //         meshMaterial.metallicFactor = metallicFactor;
                //     }
                // }
                // {
                //     float roughnessFactor;
                //     if(material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughnessFactor) == aiReturn_SUCCESS)
                //     {
                //         meshMaterial.roughnessFactor = roughnessFactor;
                //     }
                // }
                // {
                //     aiColor3D emissiveFactor;
                //     if(material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveFactor) == aiReturn_SUCCESS)
                //     {
                //         meshMaterial.emissiveFactor = glm::vec3(emissiveFactor.r, emissiveFactor.g, emissiveFactor.b);
                //     }
                // }

                //float normalScale;

                if (m_texturesEmbedded) // .glb for example
                {
                    
                    // const bool isCompressed = texture->mHeight == 0 ? true : false;
                    // if (isCompressed)
                    if (materialDiffuseCount > 0)
                    {
                        load_embedded_texture_data(material, scene, aiTextureType_BASE_COLOR, meshMaterial);
                    }
                    else
                    {
                        meshMaterial.diffuseTextureId = m_pRenderer->GetGPUTextureIdByGuid(m_pRenderer->m_missingDiffuseTextureGuid);
                    }

                    if (materialMetallicRoughnessCount > 0)
                    {
                        load_embedded_texture_data(material, scene, aiTextureType_METALNESS, meshMaterial);
                    }
                    else
                    {
                        meshMaterial.metallicRoughnessTextureId = m_pRenderer->GetGPUTextureIdByGuid(m_pRenderer->m_defaultTexturePlaceholderGuid);
                    }

                    if (materialNormalCount > 0)
                    {
                        load_embedded_texture_data(material, scene, aiTextureType_NORMALS, meshMaterial);
                    }
                    else
                    {
                        meshMaterial.normalTextureId = m_pRenderer->GetGPUTextureIdByGuid(m_pRenderer->m_defaultTexturePlaceholderGuid);
                    }

                    if (materialEmissiveCount > 0)
                    {
                        load_embedded_texture_data(material, scene, aiTextureType_EMISSIVE, meshMaterial);
                    }
                    else
                    {
                        meshMaterial.emissiveTextureId = m_pRenderer->GetGPUTextureIdByGuid(m_pRenderer->m_defaultTexturePlaceholderGuid);
                    }


                }
                else
                {
                    // TODO: When can a material have multiple textures of type diffuse?

                    // TODO: Right now this CPUModelLoader class is directly uploading the textures as it parses the assimp data structure, which doesn't
                    // necessarily follow the spirit of the class name.
                    // Better possibly would be to store the texture data and queue uploading jobs after the fact, along with uploading the mesh data.

                    // From the glTF 2.0 spec: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#metallic-roughness-material

                    // "The value for each property MAY be defined using factors and/or textures (e.g., baseColorTexture and baseColorFactor). If a texture is not given, all respective texture components within this material model MUST be assumed to have a value of 1.0. If both factors and textures are present, the factor value acts as a linear multiplier for the corresponding texture values."

                    // {
                    // "materials": [
                    //     {
                    //         "name": "Material0",
                    //         "pbrMetallicRoughness": {
                    //             "baseColorFactor": [ 0.5, 0.5, 0.5, 1.0 ],
                    //             "baseColorTexture": {
                    //                 "index": 1,
                    //                 "texCoord": 1
                    //             },
                    //             "metallicFactor": 1,
                    //             "roughnessFactor": 1,
                    //             "metallicRoughnessTexture": {
                    //                 "index": 2,
                    //                 "texCoord": 1
                    //             }
                    //         },
                    //         "normalTexture": {
                    //             "scale": 2,
                    //             "index": 3,
                    //             "texCoord": 1
                    //         },
                    //         "emissiveFactor": [ 0.2, 0.1, 0.0 ]
                    //     }
                    // ]
                    // }

                    if (materialDiffuseCount > 0) {
                        const std::string textureName {GetTextureNameFromAssimpType(material, aiTextureType_BASE_COLOR)};
                        meshMaterial.diffuseTextureId = m_textureLoader.LoadTextureFromFile(m_filePath.parent_path() / std::filesystem::path(textureName));
                    }
                    else {
                        meshMaterial.diffuseTextureId = m_pRenderer->GetGPUTextureIdByGuid(m_pRenderer->m_missingDiffuseTextureGuid);
                    }
                    if (materialMetallicRoughnessCount > 0) {
                        const std::string textureName {GetTextureNameFromAssimpType(material, aiTextureType_METALNESS)};
                        meshMaterial.metallicRoughnessTextureId = m_textureLoader.LoadTextureFromFile(m_filePath.parent_path() / std::filesystem::path(textureName));
                    }
                    else {
                        meshMaterial.metallicRoughnessTextureId = m_pRenderer->GetGPUTextureIdByGuid(m_pRenderer->m_defaultTexturePlaceholderGuid);
                    }
                    if (materialNormalCount > 0) {
                        const std::string textureName {GetTextureNameFromAssimpType(material, aiTextureType_NORMALS)};
                        meshMaterial.normalTextureId = m_textureLoader.LoadTextureFromFile(m_filePath.parent_path() / std::filesystem::path(textureName));
                    }
                    else {
                        meshMaterial.normalTextureId = m_pRenderer->GetGPUTextureIdByGuid(m_pRenderer->m_defaultTexturePlaceholderGuid);
                    }
                    if (materialEmissiveCount > 0) {
                        const std::string textureName {GetTextureNameFromAssimpType(material, aiTextureType_EMISSIVE)};
                        meshMaterial.emissiveTextureId = m_textureLoader.LoadTextureFromFile(m_filePath.parent_path() / std::filesystem::path(textureName));
                    }
                    else {
                        meshMaterial.emissiveTextureId = m_pRenderer->GetGPUTextureIdByGuid(m_pRenderer->m_defaultTexturePlaceholderGuid);
                    }
                }
                meshMaterialId = m_pRenderer->AddMaterial(meshMaterial);
                m_sceneMaterialsAlreadyLoaded.emplace(mesh->mMaterialIndex, meshMaterialId);
            }
        }
        // return cpuMesh;
    }

    glm::mat4x4 convertAssimpMatrix(const aiMatrix4x4 &aiMat)
    {
        return {
            aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
            aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
            aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
            aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
        };
    }

    void CPUModelLoader::process_assimp_node(aiNode *node, const aiScene *scene, const glm::mat4x4& accumulateMatrix)
    {
        glm::mat4x4 transform = accumulateMatrix * convertAssimpMatrix(node->mTransformation);


        // Decompose transform into its components
        // glm::vec3 scale;
        // glm::quat orientation;
        // glm::vec3 translation;
        // glm::vec3 skew;
        // glm::vec4 perspective;
        // glm::decompose(convertAssimpMatrix(node->mTransformation), scale, orientation, translation, skew, perspective);


        // Process this node's meshes
        for (size_t i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            MagicRed::Rendering::CPUMesh cpuMesh;
            MaterialId meshMaterialId{NULL_MATERIAL_ID};
            process_mesh(cpuMesh, meshMaterialId, mesh, scene, transform);
            m_cpuMeshes.push_back(cpuMesh);
            m_meshMaterialIds.push_back(meshMaterialId);
        }
        // Process this node's child node(s)
        for (size_t i = 0; i < node->mNumChildren; i++)
        {
            process_assimp_node(node->mChildren[i], scene, transform);
        }
    }

     CPUModelLoader::CPUModelLoader(
        MagicRed::Rendering::Renderer* _pRenderer
        , bool _texturesEmbedded
        , std::filesystem::path _filePath
        , std::unordered_map<std::filesystem::path, GUID>& _textureFileToGuidMapRef
    ) 
    : m_pRenderer(_pRenderer)
    , m_texturesEmbedded(_texturesEmbedded)
    , m_filePath(_filePath)
    , m_textureFileToGuidMapRef(_textureFileToGuidMapRef)
    {}

    void CPUModelLoader::LoadImmediately() {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(m_filePath, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            MRCERR("Problem loading model: " << m_filePath);
        }
        glm::mat4x4 rootTransform = convertAssimpMatrix(scene->mRootNode->mTransformation);
        process_assimp_node(scene->mRootNode, scene, rootTransform);
    }
}

POP_CLANG_WARNINGS
POP_MSVC_WARNINGS

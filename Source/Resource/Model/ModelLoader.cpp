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
#include <Rendering/Core/RenderingConfig.h>
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

    struct EmbeddedTextureData {
        std::string textureName;
        const aiTexture* texturePtr;
    };

    const std::unordered_map<aiTextureType, std::string> textureTypeToSuffixMap {
        {aiTextureType_BASE_COLOR, "_diffuse_tex"},
        {aiTextureType_METALNESS, "_metallicRoughness_tex"},
        {aiTextureType_NORMALS, "_normal_tex"},
        {aiTextureType_EMISSIVE, "_emissive_tex"}
    };

    EmbeddedTextureData GetEmbeddedTextureData(aiMaterial* material, const aiScene* scene, aiTextureType textureType, std::string modelName) {
        aiString embeddedTextureFile;
        material->GetTexture(textureType, 0, &embeddedTextureFile);
        const aiTexture* texturePtr = scene->GetEmbeddedTexture(embeddedTextureFile.C_Str());
        return {
            modelName + embeddedTextureFile.C_Str() + textureTypeToSuffixMap.at(textureType)
            , texturePtr
        };
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
            if (mesh->HasTangentsAndBitangents()) {
                vertex.tangent = glm::vec4(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, 0.0f);
            }
            if (mesh->HasTextureCoords(0)) {
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

                // Check material counts first
                std::string modelName = m_filePath.stem().string();
                if (materialDiffuseCount > 0) {
                    if (m_texturesEmbedded) {
                        EmbeddedTextureData embeddedTextureData = GetEmbeddedTextureData(material, scene, aiTextureType_BASE_COLOR, modelName);
                        meshMaterial.diffuseTextureId = m_textureLoader.LoadTextureFromFile(embeddedTextureData.textureName, embeddedTextureData.texturePtr);
                    } else {
                        const std::string textureName {GetTextureNameFromAssimpType(material, aiTextureType_BASE_COLOR)};
                        meshMaterial.diffuseTextureId = m_textureLoader.LoadTextureFromFile(m_filePath.parent_path() / std::filesystem::path(textureName));
                    }
                } else {
                    meshMaterial.diffuseTextureId = m_pRenderer->GetGPUTextureIdByGuid(m_pRenderer->m_missingDiffuseTextureGuid);
                }

                if (materialMetallicRoughnessCount > 0) {
                    if (m_texturesEmbedded) {
                        EmbeddedTextureData embeddedTextureData = GetEmbeddedTextureData(material, scene, aiTextureType_METALNESS, modelName);
                        meshMaterial.metallicRoughnessTextureId = m_textureLoader.LoadTextureFromFile(embeddedTextureData.textureName, embeddedTextureData.texturePtr);
                    } else {
                        const std::string textureName {GetTextureNameFromAssimpType(material, aiTextureType_METALNESS)};
                        meshMaterial.metallicRoughnessTextureId = m_textureLoader.LoadTextureFromFile(m_filePath.parent_path() / std::filesystem::path(textureName));
                    }
                } else {
                    meshMaterial.metallicRoughnessTextureId = m_pRenderer->GetGPUTextureIdByGuid(m_pRenderer->m_defaultTexturePlaceholderGuid);
                }

                if (materialNormalCount > 0) {
                    if (m_texturesEmbedded) {
                        EmbeddedTextureData embeddedTextureData = GetEmbeddedTextureData(material, scene, aiTextureType_NORMALS, modelName);
                        meshMaterial.normalTextureId = m_textureLoader.LoadTextureFromFile(embeddedTextureData.textureName, embeddedTextureData.texturePtr);
                    } else {
                        const std::string textureName {GetTextureNameFromAssimpType(material, aiTextureType_NORMALS)};
                        meshMaterial.normalTextureId = m_textureLoader.LoadTextureFromFile(m_filePath.parent_path() / std::filesystem::path(textureName));
                    }
                } else {
                    meshMaterial.normalTextureId = m_pRenderer->GetGPUTextureIdByGuid(m_pRenderer->m_defaultTexturePlaceholderGuid);
                }

                if (materialEmissiveCount > 0) {
                    if (m_texturesEmbedded) {
                        EmbeddedTextureData embeddedTextureData = GetEmbeddedTextureData(material, scene, aiTextureType_EMISSIVE, modelName);
                        meshMaterial.emissiveTextureId = m_textureLoader.LoadTextureFromFile(embeddedTextureData.textureName, embeddedTextureData.texturePtr);
                    } else {
                        const std::string textureName {GetTextureNameFromAssimpType(material, aiTextureType_EMISSIVE)};
                        meshMaterial.emissiveTextureId = m_textureLoader.LoadTextureFromFile(m_filePath.parent_path() / std::filesystem::path(textureName));
                    }
                } else {
                    meshMaterial.emissiveTextureId = m_pRenderer->GetGPUTextureIdByGuid(m_pRenderer->m_defaultTexturePlaceholderGuid);
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
    , m_fileToGuidMap(_textureFileToGuidMapRef)
    {}

    void DebugPrintNodeHierarchy(const aiNode* node, int depth = 0, const aiMatrix4x4& parentTransform = aiMatrix4x4()) {
        std::string indent(depth * 2, ' ');
        
        // Get this node's transformation
        aiMatrix4x4 transform = parentTransform * node->mTransformation;
        
        // Decompose the transformation matrix
        aiVector3D scaling, position;
        aiQuaternion rotation;
        transform.Decompose(scaling, rotation, position);
        
        MRLOG(indent << "Node: " << node->mName.C_Str());
        MRLOG(indent << "  Meshes: " << node->mNumMeshes);
        MRLOG(indent << "  Position: " << position.x << ", " << position.y << ", " << position.z);
        MRLOG(indent << "  Scale: " << scaling.x << ", " << scaling.y << ", " << scaling.z);
        
        // Print local transform (this node's transform only)
        aiVector3D localScaling, localPosition;
        aiQuaternion localRotation;
        node->mTransformation.Decompose(localScaling, localRotation, localPosition);
        if (localScaling.x != 1.0f || localScaling.y != 1.0f || localScaling.z != 1.0f ||
            localPosition.x != 0.0f || localPosition.y != 0.0f || localPosition.z != 0.0f) {
            MRLOG(indent << "  Local Transform:");
            MRLOG(indent << "    Position: " << localPosition.x << ", " << localPosition.y << ", " << localPosition.z);
            MRLOG(indent << "    Scale: " << localScaling.x << ", " << localScaling.y << ", " << localScaling.z);
        }
        
        // Recursively print children
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            DebugPrintNodeHierarchy(node->mChildren[i], depth + 1, transform);
        }
    }

    void CPUModelLoader::LoadImmediately() {
        Assimp::Importer importer;
        unsigned int flags = aiProcess_Triangulate
                            | aiProcess_FlipUVs 
                            | aiProcess_CalcTangentSpace
                            | aiProcess_PreTransformVertices // Flattens all nodes and their relative transforms into a single node with "frozen: transforms
                            ;
        
        const aiScene* scene = importer.ReadFile(m_filePath.string(), flags);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            MRCERR("Problem loading model: " << m_filePath);
            return;
        }
        
        MRLOG("Loading model: " << m_filePath.filename());
#if DEBUG_MODEL_NODE_HIERARCHY
        MRLOG("Node hierarchy:");
        DebugPrintNodeHierarchy(scene->mRootNode);
#endif
        
        glm::mat4x4 rootTransform = convertAssimpMatrix(scene->mRootNode->mTransformation);
        process_assimp_node(scene->mRootNode, scene, rootTransform);
    }
}

POP_CLANG_WARNINGS
POP_MSVC_WARNINGS

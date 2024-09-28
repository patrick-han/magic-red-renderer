#pragma once
#include <Rendering/Mesh/Mesh.h>
#include <Common/IdTypes.h>
#include <Rendering/Texture/TextureData.h>
#include <Rendering/Material/Material.h>
#include <Resource/TextureLoader.h>
#include <filesystem>
#include <glm/mat4x4.hpp>
#include <unordered_map>


struct aiMesh;
struct aiScene;
struct aiNode;
struct aiMaterial;
struct aiString;
#include <assimp/material.h>

namespace MagicRed::Rendering
{
    class Renderer;
}

namespace MagicRed::Resource
{
    class GUID;

    class CPUModelLoader {
    public:
        CPUModelLoader(
            MagicRed::Rendering::Renderer* _pRenderer
            , bool _texturesEmbedded
            , std::filesystem::path _filePath
            , std::unordered_map<std::filesystem::path, GUID>& _textureFileToGuidMapRef
        );
        void LoadImmediately();
        std::vector<MagicRed::Rendering::CPUMesh> m_cpuMeshes;
        std::vector<MaterialId> m_meshMaterialIds;

        // CPUMesh 
        //  |
        // MaterialGuid
        //  |            \
        //  V             V
        // TextureGuid    TextureGuid
        inline int NumberOfTexturesLoaded() {
            return static_cast<int>(m_textureFileToGuidMapRef.size());
        }
    private:
        MagicRed::Rendering::Renderer* m_pRenderer;
        bool m_texturesEmbedded;
        const std::filesystem::path m_filePath;


        
        std::unordered_map<std::filesystem::path, GUID>& m_textureFileToGuidMapRef;
        TextureLoader m_textureLoader {m_pRenderer, m_textureFileToGuidMapRef};

        // Mapping from assimp mesh material index to MaterialId to keep track
        // of materials that have already been loaded while parsing assimp structure
        std::unordered_map<unsigned int, MaterialId> m_sceneMaterialsAlreadyLoaded;

        void load_embedded_texture_data(const aiMaterial* material, const aiScene* scene, aiTextureType textureType, MagicRed::Rendering::GPUMaterial& meshMaterial);
        void process_mesh(MagicRed::Rendering::CPUMesh& cpuMesh, MaterialId& meshMaterialId, aiMesh *mesh, const aiScene *scene, const glm::mat4x4& transformMatrix);
        void process_assimp_node(aiNode *node, const aiScene *scene, const glm::mat4x4& accumulateMatrix);

    };
}

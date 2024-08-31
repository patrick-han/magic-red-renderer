#pragma once
#include <Rendering/Mesh/Mesh.h>
#include <Common/IdTypes.h>
#include <Rendering/Texture/TextureData.h>
#include <Rendering/Material/Material.h>
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
            , std::string _filePath
            , std::unordered_map<std::string, GUID>& _fileToGuidMapRef
        );
        CPUModelLoader(
            MagicRed::Rendering::Renderer* _pRenderer
            , bool _texturesEmbedded
            , std::filesystem::path _filePath
            , std::unordered_map<std::string, GUID>& _fileToGuidMapRef
        );
        void LoadImmediately();
        std::vector<MagicRed::Rendering::CPUMesh> m_cpuMeshes;
        int m_textureCount{0};
    private:
        MagicRed::Rendering::Renderer* m_pRenderer;
        bool m_texturesEmbedded;
        const std::string m_filePath; // TODO
        const std::filesystem::path m_path;
        std::unordered_map<std::string, GUID>& m_fileToGuidMapRef;

        std::unordered_map<unsigned int, MaterialId> m_sceneMaterialsAlreadyLoaded;

        


        void load_texture_from_filename(const aiMaterial* material, aiTextureType textureType, MagicRed::Rendering::GPUMaterial& meshMaterial);
        void load_embedded_texture_data(const aiMaterial* material, const aiScene* scene, aiTextureType textureType, MagicRed::Rendering::GPUMaterial& meshMaterial);
        MagicRed::Rendering::CPUMesh process_mesh(aiMesh *mesh, const aiScene *scene, const glm::mat4x4& transformMatrix);
        void process_assimp_node(aiNode *node, const aiScene *scene, const glm::mat4x4& accumulateMatrix);

    };
}

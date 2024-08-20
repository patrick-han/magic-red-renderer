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
    class CPUModelLoader {
    public:
        CPUModelLoader(std::string _filePath, bool _texturesEmbedded, MagicRed::Rendering::Renderer* _pRenderer);
        void LoadImmediately();
        std::vector<MagicRed::Rendering::CPUMesh> m_cpuMeshes;
    private:
        MagicRed::Rendering::Renderer* m_pRenderer;
        bool m_texturesEmbedded;
        const std::string m_filePath; // TODO
        const std::filesystem::path m_path;


        std::unordered_map<unsigned int, MaterialId> m_sceneMaterialsAlreadyLoaded;

        inline static const std::string missingDiffuseTextureName{"missing_diffuse_texture.png"};
        inline static const std::string default1TextureName{"default_1_texture.png"};


        void load_texture_from_filename(const aiMaterial* material, aiTextureType textureType, MagicRed::Rendering::Material& meshMaterial);
        void load_embedded_texture_data(const aiMaterial* material, const aiScene* scene, aiTextureType textureType, MagicRed::Rendering::Material& meshMaterial);
        MagicRed::Rendering::CPUMesh process_mesh(aiMesh *mesh, const aiScene *scene, const glm::mat4x4& transformMatrix);
        void process_assimp_node(aiNode *node, const aiScene *scene, const glm::mat4x4& accumulateMatrix);

    };
}

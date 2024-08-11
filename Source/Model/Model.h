#pragma once
#include <Rendering/Mesh/Mesh.h>
#include <Common/IdTypes.h>
#include <Rendering/Texture/TextureData.h>
#include <Rendering/Material/Material.h>
#include <filesystem>

struct aiMesh;
struct aiScene;
struct aiNode;
struct aiMaterial;
struct aiString;
#include <assimp/material.h>
#include <glm/mat4x4.hpp>
#include <unordered_map>
#include <Common/IdTypes.h>

namespace MagicRed::Rendering
{
    class GfxDevice;
    class TextureCache;
    class MaterialCache;
}

namespace MagicRed::Asset
{
    struct CPUModel {
        CPUModel(const char* _filePath, bool _texturesEmbedded, MagicRed::Rendering::MaterialCache& _materialCache, MagicRed::Rendering::TextureCache& _textureCache, const MagicRed::Rendering::GfxDevice& _gfxDevice);

        std::vector<MagicRed::Rendering::CPUMesh> m_cpuMeshes;
    private:
        MagicRed::Rendering::MaterialCache& m_materialCache;
        MagicRed::Rendering::TextureCache& m_textureCache;
        const MagicRed::Rendering::GfxDevice& m_gfxDevice;
        bool m_texturesEmbedded;
        const char* m_filePath;
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

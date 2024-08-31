#include "ResourceManager.h"
#include <Resource/Model/Model.h>
#include <Rendering/Mesh/Mesh.h>
#include <Rendering/Core/Renderer.h>
// #include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <External/json.hpp>
#include <fstream>
#include <cstring>
#include <Common/Compiler/Unused.h>
#include <Resource/AssetMaker.h>
#include <cassert>

using json = nlohmann::json;

namespace MagicRed::Resource
{
    ResourceManager::ResourceManager() {};
    ResourceManager::~ResourceManager() {};
    void ResourceManager::Shutdown() {

    }

    void ResourceManager::Startup/*RegisterRenderer*/(MagicRed::Rendering::Renderer* _pRenderer) {
        m_pRenderer = _pRenderer;
    }

    void ResourceManager::SetProjectDirectory(const std::filesystem::path& projectDirectory) {
        m_projectDirectory = projectDirectory;
    }

    void ResourceManager::ImportModel(const std::filesystem::path& sourceLocalFilePath, bool texturesEmbedded) {
        // Check to make sure the model isn't already an imported asset next to the file
        std::filesystem::path sourceAbsoluteFilePath = m_projectDirectory / sourceLocalFilePath;
        std::filesystem::path assetFilePath(sourceAbsoluteFilePath);
        assetFilePath += ".asset";
        if(std::filesystem::exists(assetFilePath))
        {
            LoadModel(assetFilePath);
            return;
        }


        // If not, start the process of generating a new .asset
        GUID modelGUID;
        AssetMaker modelAssetMaker(modelGUID, AssetType::Model, sourceAbsoluteFilePath.string());
        modelAssetMaker.AddTexturesEmbeddedField(texturesEmbedded);

        // TODO: DUPLICATE CODE
        CPUModelLoader cpuModelLoader(m_pRenderer, texturesEmbedded, sourceAbsoluteFilePath, m_fileToGuidMap);
        cpuModelLoader.LoadImmediately();
        glm::mat4 translate = glm::translate(glm::mat4{ 1.0f }, glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(550.0f, 550.0f, 550.0f));

        assert(cpuModelLoader.m_cpuMeshes.size() == cpuModelLoader.m_meshMaterialIds.size());
        for (size_t i = 0; i < cpuModelLoader.m_cpuMeshes.size(); i++)
        {
            MagicRed::Rendering::CPUMesh& mesh = cpuModelLoader.m_cpuMeshes[i];
            MaterialId& meshMaterialId = cpuModelLoader.m_meshMaterialIds[i];
            GPUMeshId meshId = m_pRenderer->UploadMesh(mesh, meshMaterialId);
            m_pRenderer->m_sceneRenderMeshComponents.emplace_back(meshId, m_pRenderer->m_MeshCache, translate * scale);
        }

        modelAssetMaker.AddTextureCount(cpuModelLoader.NumberOfTexturesLoaded());
        modelAssetMaker.Write(assetFilePath);
    }


    void ResourceManager::LoadModel(std::filesystem::path assetFilePath) {

        std::ifstream jsonFileStream(assetFilePath);

        // Extract asset fields
        json jsonData = json::parse(jsonFileStream);
        if (std::strcmp(jsonData["assetType"].dump().c_str(), "\"model\""))
        {
            MRLOG("Tried to load a non model assetType");
            exit(1);
        }
        bool texturesEmbedded = jsonData["texturesEmbedded"].template get<bool>();
        GUID guid(jsonData["guid"]);
        std::string sourceFilePath = jsonData["fileSource"].dump();
        // Erase the escaped quotes at the front and back of the fileSource field
        sourceFilePath.erase(0, 1);
        sourceFilePath.erase(sourceFilePath.size() - 1);
        MRLOG("Importing model asset: " << guid);

        // TODO: DUPLICATE CODE
        CPUModelLoader cpuModelLoader(m_pRenderer, texturesEmbedded, m_projectDirectory / sourceFilePath, m_fileToGuidMap);
        cpuModelLoader.LoadImmediately();
        glm::mat4 translate = glm::translate(glm::mat4{ 1.0f }, glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(550.0f, 550.0f, 550.0f));

        assert(cpuModelLoader.m_cpuMeshes.size() == cpuModelLoader.m_meshMaterialIds.size());
        for (size_t i = 0; i < cpuModelLoader.m_cpuMeshes.size(); i++)
        {
            MagicRed::Rendering::CPUMesh& mesh = cpuModelLoader.m_cpuMeshes[i];
            MaterialId& meshMaterialId = cpuModelLoader.m_meshMaterialIds[i];
            GPUMeshId meshId = m_pRenderer->UploadMesh(mesh, meshMaterialId);
            m_pRenderer->m_sceneRenderMeshComponents.emplace_back(meshId, m_pRenderer->m_MeshCache, translate * scale);
        }

    }
}

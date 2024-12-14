#include "ResourceManager.h"
#include <Resource/Model/ModelLoader.h>
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

    void ResourceManager::ImportModel(const std::filesystem::path& sourceLocalFilePath, bool texturesEmbedded, glm::vec3 translate, glm::vec3 scale) {
        std::filesystem::path sourceAbsoluteFilePath = m_projectDirectory / sourceLocalFilePath;
        CPUModelLoader cpuModelLoader(m_pRenderer, texturesEmbedded, sourceAbsoluteFilePath, m_fileToGuidMap);
        cpuModelLoader.LoadImmediately();
        glm::mat4 translateMat = glm::translate(glm::mat4{ 1.0f }, translate);
        glm::mat4 scaleMat = glm::scale(translateMat, scale);

        assert(cpuModelLoader.m_cpuMeshes.size() == cpuModelLoader.m_meshMaterialIds.size());
        for (size_t i = 0; i < cpuModelLoader.m_cpuMeshes.size(); i++)
        {
            MagicRed::Rendering::CPUMesh& mesh = cpuModelLoader.m_cpuMeshes[i];
            MaterialId& meshMaterialId = cpuModelLoader.m_meshMaterialIds[i];
            GPUMeshId meshId = m_pRenderer->UploadMesh(mesh, meshMaterialId);
            m_pRenderer->m_sceneRenderMeshComponents.emplace_back(meshId, m_pRenderer->m_MeshCache, translateMat * scaleMat);
        }
    }
}

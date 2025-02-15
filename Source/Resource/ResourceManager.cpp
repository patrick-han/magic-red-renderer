#include "ResourceManager.h"
#include <Resource/Model/ModelLoader.h>
#include <Rendering/Mesh/Mesh.h>
#include <Rendering/Core/Renderer.h>

#include <External/json.hpp>
#include <fstream>
#include <cstring>
#include <Common/Compiler/Unused.h>
#include <Resource/AssetMaker.h>
#include <cassert>

#include <Common/Math/Matrix4f.h>

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

    void ResourceManager::ImportModel(const std::filesystem::path& sourceLocalFilePath, bool texturesEmbedded, const Vector3f& translate, const Vector3f& scale) {
        std::filesystem::path sourceAbsoluteFilePath = m_projectDirectory / sourceLocalFilePath;
        CPUModelLoader cpuModelLoader(m_pRenderer, texturesEmbedded, sourceAbsoluteFilePath, m_fileToGuidMap);
        cpuModelLoader.LoadImmediately();

        Matrix4f translateMat = Matrix4f::MakeTranslate(translate);
        Matrix4f scaleMat = Matrix4f::MakeScale(scale);

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

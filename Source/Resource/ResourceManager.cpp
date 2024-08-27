#include "ResourceManager.h"
#include <Resource/Model/Model.h>
#include <Rendering/Mesh/Mesh.h>
#include <Rendering/Core/Renderer.h>
// #include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace MagicRed::Resource
{
    ResourceManager::ResourceManager() {};
    ResourceManager::~ResourceManager() {};
    void ResourceManager::Shutdown() {

    }

    void ResourceManager::Startup/*RegisterRenderer*/(MagicRed::Rendering::Renderer* _pRenderer) {
        m_pRenderer = _pRenderer;
    }


    // {
    //         // Sponza mesh
    //         MagicRed::Asset::CPUModel sponzaModel(ROOT_DIR "/Assets/Meshes/sponza-gltf/Sponza.gltf", false, m_MaterialCache, m_TextureCache, m_GfxDevice);
    //         glm::mat4 translate = glm::translate(glm::mat4{ 1.0f }, glm::vec3(0.0f, 0.0f, 0.0f));
    //         //    glm::mat4 rotate = glm::rotate(translate, rm, glm::vec3(0.0, 0.0, 1.0));
    //         glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(550.0f, 550.0f, 550.0f));
    //         for (CPUMesh& mesh : sponzaModel.m_cpuMeshes)
    //         {
    //             GPUMeshId sponzaMeshId = m_MeshCache.add_mesh(m_GfxDevice, mesh);
    //             m_sceneRenderMeshComponents.emplace_back(sponzaMeshId, m_MeshCache, translate * scale);
    //         }
    //     }

    void ResourceManager::LoadModel(std::string filePath, bool texturesEmbedded) {
        CPUModelLoader cpuModelLoader(m_pRenderer, texturesEmbedded, filePath, m_guidToFileMap);
        cpuModelLoader.LoadImmediately();
        glm::mat4 translate = glm::translate(glm::mat4{ 1.0f }, glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(550.0f, 550.0f, 550.0f));
        for (MagicRed::Rendering::CPUMesh& mesh : cpuModelLoader.m_cpuMeshes)
        {
            GPUMeshId meshId = m_pRenderer->UploadMesh(mesh);
            m_pRenderer->m_sceneRenderMeshComponents.emplace_back(meshId, m_pRenderer->m_MeshCache, translate * scale);
        }
    }
}

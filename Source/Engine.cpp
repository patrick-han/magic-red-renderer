#include "Engine.h"
#include <Common/RootDir.h>
#include <filesystem>


namespace MagicRed
{

    Engine::Engine() {}
    Engine::~Engine() {}
    void Engine::Startup() {
        m_renderer.Startup();
        m_resourceManager.Startup(&m_renderer);
    }

    void Engine::Run() {
        std::filesystem::path gameDirectory(ROOT_DIR "Game/");
        m_resourceManager.SetProjectDirectory(gameDirectory);
        m_renderer.SetResourceManager(&m_resourceManager);

        // Load Resources
        std::filesystem::path sponzaPath("sponza-gltf/Sponza.gltf");
        m_resourceManager.ImportModel(sponzaPath, false, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(550.0f, 550.0f, 550.0f));

        std::filesystem::path helmetPath("damagedHelmet-glb/DamagedHelmet.glb");
        m_resourceManager.ImportModel(helmetPath, true, glm::vec3(0.0f, 6.0f, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f));

        // std::filesystem::path helmetGltfPath("damagedHelmet-gltf/DamagedHelmet.gltf");
        // m_resourceManager.ImportModel(helmetGltfPath, false);

//        std::filesystem::path beautifulGamePath("ABeautifulGame/ABeautifulGame.gltf");
//        m_resourceManager.ImportModel(beautifulGamePath, false, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));


        m_renderer.run();
    }

    void Engine::Shutdown() {
        m_resourceManager.Shutdown();
        m_renderer.Shutdown();
    }
}

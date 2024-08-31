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

        // Load Resources
        std::filesystem::path sponzaPath("sponza-gltf/Sponza.gltf");
        m_resourceManager.ImportModel(sponzaPath, false);


        m_renderer.run();
    }

    void Engine::Shutdown() {
        m_resourceManager.Shutdown();
        m_renderer.Shutdown();
    }
}
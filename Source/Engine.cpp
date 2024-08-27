#include "Engine.h"
#include <Common/RootDir.h>


namespace MagicRed
{

    Engine::Engine() {}
    Engine::~Engine() {}
    void Engine::Startup() {
        m_renderer.Startup();
        m_resourceManager.Startup(&m_renderer);
    }

    void Engine::Run() {

        // Load Resources
        std::string sponzaPathString = std::string(ROOT_DIR "/Assets/Meshes/sponza-gltf/Sponza.gltf");
        m_resourceManager.LoadModel(sponzaPathString, false);



        m_renderer.run();
    }

    void Engine::Shutdown() {
        m_resourceManager.Shutdown();
        m_renderer.Shutdown();
    }
}
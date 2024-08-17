#include "Engine.h"



namespace MagicRed
{

    Engine::Engine() {}
    Engine::~Engine() {}
    void Engine::Startup() {
        m_renderer.Startup();
    }

    void Engine::Run() {
        m_renderer.run();
    }

    void Engine::Shutdown() {
        m_renderer.Shutdown();
    }
}
#pragma once
#include <Rendering/Core/Renderer.h>
#include <Resource/ResourceManager.h>


namespace MagicRed
{
    class Engine {
    public:
        Engine();
        ~Engine();
        void Startup();
        void Run();
        void Shutdown();
    private:
        Rendering::Renderer m_renderer;
        Resource::ResourceManager m_resourceManager;
    };
}


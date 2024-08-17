#pragma once
#include <Rendering/Core/Renderer.h>


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
    };
}


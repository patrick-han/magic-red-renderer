#pragma once
#include <string>
#include <Resource/GUID.h>

namespace MagicRed::Rendering
{
    class Renderer;
}


namespace MagicRed::Resource 
{
    class ResourceManager {
    public:
        ResourceManager();
        ~ResourceManager();
        // void Startup();
        void Shutdown();

        // Although the renderer manages Meshes and Textures itself, since it "owns" the GPU,
        // the API for doing so should still be through the ResourceManager. The user should
        // not have to interface directly with the Rendering engine. The job of the
        // ResourceManager is to dispatch resource requests to/between the various engine 
        // systems
        void Startup/*RegisterRenderer*/(MagicRed::Rendering::Renderer* _pRenderer);
        void LoadModel(std::string filePath, bool texturesEmbedded);

    private:
        MagicRed::Rendering::Renderer* m_pRenderer {nullptr};
        std::unordered_map<GUID, std::string> m_guidToFileMap;


    };
}


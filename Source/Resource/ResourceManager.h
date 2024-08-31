#pragma once
#include <string>
#include <Resource/GUID.h>
#include <filesystem>

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
        void SetProjectDirectory(const std::filesystem::path& projectDirectory);

        // Import is used for models not imported already. i.e. they do not yet have an .asset file associated with them already
        void ImportModel(const std::filesystem::path& sourceLocalFilePath, bool texturesEmbedded);

    private:
        // In contrast, Load is used for models that already have an .asset
        void LoadModel(std::filesystem::path assetFilePath);
        MagicRed::Rendering::Renderer* m_pRenderer {nullptr};

        std::filesystem::path m_projectDirectory;

        // TODO: Eventually textures should be compiled on disk with permanent guids assigned?
        std::unordered_map<std::string, GUID> m_fileToGuidMap;


    };
}


#pragma once
#include <string>
#include <Resource/GUID.h>
#include <filesystem>
#include <glm/vec3.hpp>

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

        void ImportModel(const std::filesystem::path& sourceLocalFilePath, bool texturesEmbedded, glm::vec3 translate, glm::vec3 scale);

    private:
        MagicRed::Rendering::Renderer* m_pRenderer {nullptr};

        std::filesystem::path m_projectDirectory;

        // TODO: Eventually textures should be compiled on disk with permanent guids assigned?
        std::unordered_map<std::filesystem::path, GUID> m_fileToGuidMap;


    };
}


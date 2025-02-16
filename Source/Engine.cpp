#include "Engine.h"
#include <Common/RootDir.h>
#include <filesystem>
#include <Common/Math/Vector3f.h>
#include <Common/Log.h>

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


        Vector3f a(1.0f, 2.0f, 3.0f);

        Vector3f b(3.0f, 2.0f, 1.0f);

        Vector3f c = 3 * a;
        Vector3f d = a * 3;

        MRLOG(std::format("c: {}, {}, {}", c.x, c.y, c.z));
        MRLOG(std::format("d: {}, {}, {}", d.x, d.y, d.z));

        a = a.AsNormalized();
        MRLOG(std::format("a: {}, {}, {}", a.x, a.y, a.z));
        // exit(1);
        // Load Resources
         std::filesystem::path sponzaPath("sponza-gltf/Sponza.gltf");
         m_resourceManager.ImportModel(sponzaPath, false, Vector3f(0.0f, 0.0f, 0.0f), Vector3f(5.0f, 5.0f, 5.0f));

        //std::filesystem::path beautifulGamePath("ABeautifulGame/ABeautifulGame.gltf");
        //m_resourceManager.ImportModel(beautifulGamePath, false, Vector3f(5.0f, 6.0f, 0.0f), Vector3f(2.0f, 2.0f, 2.0f));

        std::filesystem::path orientationTestPath("OrientationTest.glb");
        m_resourceManager.ImportModel(orientationTestPath, true, Vector3f(0.0f, 10.f, -3.0f), Vector3f(0.25f, 0.25f, 0.25f));

        // std::filesystem::path helmetPath("damagedHelmet-glb/DamagedHelmet.glb");
        // m_resourceManager.ImportModel(helmetPath, true, Vector3f(0.0f, 6.0f, 0.0f), Vector3f(2.0f, 2.0f, 2.0f));

        std::filesystem::path helmetGltfPath("damagedHelmet-gltf/DamagedHelmet.gltf");
        m_resourceManager.ImportModel(helmetGltfPath, false, Vector3f(0.0f, 6.0f, 0.0f), Vector3f(2.0f, 2.0f, 2.0f));


        m_renderer.run();
    }

    void Engine::Shutdown() {
        m_resourceManager.Shutdown();
        m_renderer.Shutdown();
    }
}

#include <Engine.h>




int main() {
    MagicRed::Engine engine;

    engine.Startup();

    engine.Run();

    engine.Shutdown();

    return EXIT_SUCCESS;
}
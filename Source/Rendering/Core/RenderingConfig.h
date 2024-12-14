#pragma once
#include <cstdlib>
#include <Common/Platform.h>

namespace MagicRed::Rendering
{
#if PLATFORM_WINDOWS
    inline constexpr uint32_t WINDOW_WIDTH = 2500;
    inline constexpr uint32_t WINDOW_HEIGHT = 1200;
#else
    inline constexpr uint32_t WINDOW_WIDTH = 1500;
    inline constexpr uint32_t WINDOW_HEIGHT = 800;
#endif
    inline constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    inline constexpr uint32_t SHADOWMAP_RESOLUTION = 1024;
}

#define DEBUG_DIRECTIONAL_LIGHT 0
#define DEBUG_MODEL_NODE_HIERARCHY 0
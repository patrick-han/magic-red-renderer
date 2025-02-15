#pragma once
#include <numbers>
namespace MagicRed {
float deg2rad(float degrees) {
    return degrees * (std::numbers::pi / 180.0f);
}
}
#include "GUID.h"

#include <random>


namespace MagicRed::Resource
{

    static std::random_device s_RandomDevice;
    static std::mt19937_64 s_Engine(s_RandomDevice());
    static std::uniform_int_distribution<std::uint64_t> s_UniformDistribution;

    GUID::GUID() : m_GUID(s_UniformDistribution(s_Engine))
    {
        
    }

    GUID::GUID(std::uint64_t _GUID) : m_GUID(_GUID)
    {
    }
}
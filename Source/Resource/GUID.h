#pragma once
#include <cstdint>
#include <cstddef>
#include <functional>
// Based on:
// https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Core/UUID.h

namespace MagicRed::Resource 
{
    class ResourceManager;

    class GUID {
    public:
        GUID();
        GUID(std::uint64_t _GUID);
        GUID(const GUID&) = default;
        GUID(GUID&&) = delete;

        [[nodiscard]] operator uint64_t() const 
        { 
            return m_GUID;
        }
        [[nodiscard]] std::string to_string() const
        {
            return std::to_string(m_GUID);
        }
    private:
        std::uint64_t m_GUID;
    };
}   

namespace std // Need a hashing function for GUIDs to use in containers
{
//    template <typename T> struct hash;

    template<>
    struct hash<MagicRed::Resource::GUID>
    {
        std::size_t operator()(const MagicRed::Resource::GUID& guid) const {
            return hash<uint64_t>()((uint64_t)guid);
        }
    };
}

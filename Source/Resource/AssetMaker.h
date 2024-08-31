#pragma once
#include <string>
#include <External/json.hpp>
#include <Resource/GUID.h>
#include <Common/Log.h>
#include <span>

namespace MagicRed::Resource
{

    enum class AssetType {
        Model,
        Texture,
        Material,
        AssetDatabase
    };

    class AssetMaker {

        static constexpr const char* fieldEnd = ",\n";
    public:
        AssetMaker() = delete;
        AssetMaker(const GUID guid, const AssetType assetType, const std::string& fileSource);
        AssetMaker(const AssetMaker&) = delete;
        AssetMaker(AssetMaker&&) = delete;
        AssetMaker& operator=(const AssetMaker&) = delete;
        AssetMaker& operator=(AssetMaker&&) = delete;
        ~AssetMaker();

        void AddTexturesEmbeddedField(bool value);
        void AddTextureGUIDArray(const std::span<GUID const> textureGuids);
        void AddMaterialParameters(
            const GUID diffuseTexGuid
            , const GUID normalTexGuid
            , const GUID metallicRoughnessTexGuid
            , const GUID emissiveTexGuid
        );
        void AddTextureCount(int value);

        void Write(std::filesystem::path assetFilePath);

        inline void PrintCurrentOutput() {
            MRLOG(m_output);
        }


    private:
        std::string m_output{"{\n"};
    };

}

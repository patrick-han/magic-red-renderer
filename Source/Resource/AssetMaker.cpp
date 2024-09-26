#include "AssetMaker.h"
#include <External/json.hpp>
#include <fstream>
using json = nlohmann::json;

#define LINE_ENDER ",\n"

namespace MagicRed::Resource
{
    std::string AssetMaker::AssetTypeToString(AssetType assetType) const {
        static const std::unordered_map<AssetType, std::string> assetTypeMap = {
            {AssetType::Model, "model"},
            {AssetType::Texture, "texture"},
            {AssetType::Material, "material"},
            {AssetType::AssetDatabase, "assetDatabase"}
        };

        return assetTypeMap.at(assetType);
    }


    AssetMaker::AssetMaker(const GUID guid, const AssetType assetType, const std::string& fileSource) {

        m_output["guid"] = guid.to_string();
        m_output["assetType"] = AssetTypeToString(assetType);
        m_output["fileSource"] = fileSource;
    }

    AssetMaker::~AssetMaker() {}

    void AssetMaker::AddTexturesEmbeddedField(bool value) {
        m_output["texturesEmbedded"] = value;
    }

    void AssetMaker::AddGUIDArray(const char* fieldName, const std::span<GUID const> guids) {
        std::vector<std::string> guidStrings;
        for (GUID guid : guids)
        {
            guidStrings.push_back(guid.to_string());
        }
        m_output[fieldName] = guidStrings;
    }

    void AssetMaker::AddMaterialParameters(
          const GUID diffuseTexGuid
        , const GUID normalTexGuid
        , const GUID metallicRoughnessTexGuid
        , const GUID emissiveTexGuid
    ) {
        // Add guids for each texture map mirroring that of the definition in Material.h
        // TODO: For now, just the 4 maps without other attributes:
        m_output["diffuseTex"] = diffuseTexGuid.to_string();
        m_output["normalTex"] = normalTexGuid.to_string();
        m_output["metallicRoughnessTex"] = metallicRoughnessTexGuid.to_string();
        m_output["emissiveTex"] = emissiveTexGuid.to_string();
    }

    void AssetMaker::AddTextureCount(int value) {
        m_output["textureCount"] = value;
    }

    void AssetMaker::Write(std::filesystem::path assetFilePath) {
        std::ofstream file(assetFilePath);
        file << std::setw(4)<< m_output;
        file.close();
    }

}

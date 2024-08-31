#include "AssetMaker.h"
#include <External/json.hpp>
#include <fstream>
using json = nlohmann::json;

#define LINE_ENDER ",\n"

namespace MagicRed::Resource
{
    AssetMaker::AssetMaker(const GUID guid, const AssetType assetType, const std::string& fileSource) {
        m_output += "\t\"guid\": " + guid.to_string() + LINE_ENDER;
        m_output += "\t\"assetType\": ";
        switch(assetType)
        {
            case AssetType::Model:
                m_output += "\"model\"" LINE_ENDER;
                break;
            case AssetType::Texture:
                m_output += "\"texture\"" LINE_ENDER;
                break;
            case AssetType::Material:
                m_output += "\"material\"" LINE_ENDER;
                break;
            case AssetType::AssetDatabase:
                m_output += "\"assetDatabase\"" LINE_ENDER;
                break;
        }
        
        m_output += "\t\"fileSource\": \"" + fileSource + "\"" LINE_ENDER;
    }

    AssetMaker::~AssetMaker() {}

    void AssetMaker::AddTexturesEmbeddedField(bool value) {
        m_output += "\t\"texturesEmbedded\": " + std::string(value ? "true" LINE_ENDER : "false" LINE_ENDER);
    }

    void AssetMaker::AddGUIDArray(const char* fieldName, const std::span<GUID const> guids) {
        m_output += fieldName;
        m_output += ": [";
        for (GUID guid : guids)
        {
            m_output += guid.to_string();
            m_output += ", ";
        }
        m_output.erase(m_output.size() - 1); // Remove trailing comma of last element;
        m_output += "] " LINE_ENDER;
    }

    void AssetMaker::AddMaterialParameters(
        const GUID diffuseTexGuid
        , const GUID normalTexGuid
        , const GUID metallicRoughnessTexGuid
        , const GUID emissiveTexGuid
    ) {
        // Add guids for each texture map mirroring that of the definition in Material.h
        // TODO: For now, just the 4 maps without other attributes:
        m_output += "\t\"diffuseTex\": " + diffuseTexGuid.to_string() + LINE_ENDER;
        m_output += "\t\"normalTex\": " + normalTexGuid.to_string() + LINE_ENDER;
        m_output += "\t\"metallicRoughnessTex\": " + metallicRoughnessTexGuid.to_string() + LINE_ENDER;
        m_output += "\t\"emissiveTex\": " + emissiveTexGuid.to_string() + LINE_ENDER;
    }

    void AssetMaker::AddTextureCount(int value) {
        m_output += "\t\"textureCount\": " + std::to_string(value) + LINE_ENDER;
    }

    void AssetMaker::Write(std::filesystem::path assetFilePath) {
        m_output.erase(m_output.size() - 2); // Remove trailing comma and newline on the last field
        m_output += "\n}";

        json jsonData = json::parse(m_output);
        std::ofstream file(assetFilePath);
        file << std::setw(4)<< jsonData;
        file.close();
    }

}

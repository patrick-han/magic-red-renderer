// #include <Resource/TextureLoader.h>
// #include <Resource/GUID.h>
// #include <External/stb_image.h>
// #include <Rendering/Texture/TextureData.h>

// namespace MagicRed::Resource
// {
//     TextureLoader::TextureLoader()
//     {

//     }

//     GPUTextureId TextureLoader::LoadTextureFromFile(const std::filesystem::path& texturePath)
//     {
//         GPUTextureId gpuTextureId;

//         // Now, we actually load the texture file if it hasn't already been loaded and uploaded to the GPU
//         if (m_textureFileToGuidMapRef.count(texturePath) == 0)
//         {
//             GUID newTextureGuid = GUID();
//             int width, height, numberComponents;
//             unsigned char *data = stbi_load(texturePath.string().c_str(), &width, &height, &numberComponents, STBI_rgb_alpha); // TODO: request 4 channels from all images

//             MagicRed::Rendering::TextureLoadingData textureLoadingData = {
//                 .data = data,
//                 .texSize = {width, height, 4} // TODO: force all images to have 4 channels...ignoring numberComponents for now
//             };
//             m_textureFileToGuidMapRef.insert({texturePath, newTextureGuid});
//             gpuTextureId = m_pRenderer->UploadTexture(textureLoadingData, newTextureGuid);
//             stbi_image_free(data);
//         }
//         else // Otherwise retrieve the existing guid...
//         {
//             gpuTextureId = m_pRenderer->GetGPUTextureIdByGuid(m_textureFileToGuidMapRef[textureName];);
//         }
//         return gpuTextureId;
//     }
// }
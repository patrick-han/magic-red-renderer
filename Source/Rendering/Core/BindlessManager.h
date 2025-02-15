#pragma once

#include "GfxDevice.h"

namespace MagicRed::Rendering {

    class GfxDevice;
    class TextureCache;

    class BindlessManager {
        
        public:
            BindlessManager(GfxDevice& device, TextureCache& textureCache);
            ~BindlessManager();
            void Cleanup();

            void InitBindlessDescriptors();
            void UpdateBindlessTextures();
            void UpdateBindlessSamplers(VkSampler linearSampler, VkSampler shadowSampler);
            inline VkDescriptorSet GetBindlessDescriptorSet() const { return m_bindlessDescriptorSet; }
            inline VkDescriptorPool GetBindlessDescriptorPool() const { return m_bindlessDescriptorPool; }
            inline VkDescriptorSetLayout GetBindlessDescriptorSetLayout() const { return m_bindlessDescriptorSetLayout; }

        private:
            GfxDevice& m_GfxDevice;
            TextureCache& m_TextureCache;
            VkDescriptorSet m_bindlessDescriptorSet;
            VkDescriptorPool m_bindlessDescriptorPool;
            VkDescriptorSetLayout m_bindlessDescriptorSetLayout;
            
    };
}

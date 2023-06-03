#ifndef STRAITX_VULKAN_TEXTURE_IMPL_HPP
#define STRAITX_VULKAN_TEXTURE_IMPL_HPP

#include "graphics/api/vulkan/vulkan.hpp"
#include "graphics/api/texture.hpp"

namespace Vk{

VkFormat ToVkFormat(TextureFormat format);

VkImageLayout ToVkLayout(TextureLayout layout);

VkImageUsageFlags ToVkImageUsage(TextureUsage usage);

VkSampleCountFlags ToVkSamplesCount(SamplePoints points);

class Texture2DImpl: public Texture2D{
private:
    VkImage m_Handle = VK_NULL_HANDLE;
    VkImageView m_View = VK_NULL_HANDLE;
    VkDeviceMemory m_Memory = VK_NULL_HANDLE;
public:
    Texture2DImpl(u32 width, u32 height, TextureFormat format, TextureUsage usage, TextureLayout initial_layout);

    Texture2DImpl(VkImage image, u32 width, u32 height, TextureFormat image_format, TextureUsage image_usage, TextureLayout image_layout);

    ~Texture2DImpl();

    operator VkImage()const{
        return Handle();
    }

    VkImage Handle()const{
        return m_Handle;
    }

    VkImageView ViewHandle()const{
        return m_View;
    }

    void ChangeLayout(TextureLayout new_layout)const override;
    
    void Copy(void *src_data, Vector2u src_size)override;
private:
    void CreateImage();

    void CreateImageView();

    void DestroyImageView();

    void DestroyImage();

    bool IsCreatedFromImage()const{
        return m_Memory == VK_NULL_HANDLE;
    }
};

}//namespace Vk::

#endif//STRAITX_VULKAN_TEXTURE_IMPL_HPP
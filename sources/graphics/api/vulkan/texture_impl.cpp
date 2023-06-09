#include "graphics/api/vulkan/texture_impl.hpp"
#include "graphics/api/vulkan/gpu_impl.hpp"
#include "graphics/api/vulkan/immediate.hpp"
#include "graphics/api/vulkan/buffer_impl.hpp"
#include "graphics/api/vulkan/memory_allocator.hpp"
#include "core/print.hpp"

namespace Vk{

VkFormat ToVkFormat(TextureFormat format){
    switch (format) {
    case TextureFormat::Unknown:         return VK_FORMAT_UNDEFINED;
    case TextureFormat::RGBA8:           return VK_FORMAT_R8G8B8A8_UNORM;
    case TextureFormat::RGB8:            return VK_FORMAT_R8G8B8_UNORM;
    case TextureFormat::RGBA16F:         return VK_FORMAT_R16G16B16A16_SFLOAT;
    case TextureFormat::BGRA8:           return VK_FORMAT_B8G8R8A8_UNORM;
    case TextureFormat::Depth32:         return VK_FORMAT_D32_SFLOAT;
    case TextureFormat::Depth24Stencil8: return VK_FORMAT_D24_UNORM_S8_UINT;
    }
    SX_CORE_ASSERT(false, "unhandled texture format");
    return VK_FORMAT_UNDEFINED;
}

VkImageLayout ToVkLayout(TextureLayout layout){
    static VkImageLayout s_LayoutTable[] = {
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };

    return s_LayoutTable[(size_t)layout];
}

VkImageUsageFlags ToVkImageUsage(TextureUsage usage){
    return VkImageUsageFlags(usage);
}

VkSampleCountFlags ToVkSamplesCount(SamplePoints points){
    return 1 << (u32)points;
}

Texture2DImpl::Texture2DImpl(u32 width, u32 height, TextureFormat format, TextureUsage usage, TextureLayout initial_layout){
    m_Format = format;
    m_Usage = usage;
    m_Layout = TextureLayout::Undefined;
    m_Width = width;
    m_Height = height;
    CreateImage();
    CreateImageView();

    ChangeLayout(initial_layout);
}

Texture2DImpl::Texture2DImpl(VkImage image, u32 width, u32 height, TextureFormat image_format, TextureUsage image_usage, TextureLayout image_layout){
    m_Format = image_format;
    m_Usage = image_usage;
    m_Layout = image_layout;
    m_Width = width;
    m_Height = height;
    
    m_Handle = image;
    CreateImageView();
}

Texture2DImpl::~Texture2DImpl(){
    DestroyImageView();

    if(!IsCreatedFromImage())
        DestroyImage();
}
void Texture2DImpl::Copy(void *src_data, Vector2u src_size){
    BufferImpl tmp(src_size.x * src_size.y * GetPixelSize(Format()), BufferMemoryType::UncachedRAM, BufferUsageBits::TransferDestination | BufferUsageBits::TransferSource);
    tmp.Copy(src_data, tmp.Size(), 0);

    Immediate::Copy(&tmp, this);
}

void Texture2DImpl::ChangeLayout(TextureLayout new_layout)const{
    Immediate::ChangeLayout(this, new_layout);
}

void Texture2DImpl::CreateImage(){
    SX_CORE_ASSERT(m_Format != TextureFormat::Unknown,"GPUTexture: Can't be created with Format::Unknown");

    VkImageCreateInfo info;
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = ToVkFormat(m_Format);
    info.extent.width = m_Width;
    info.extent.height = m_Height;
    info.extent.depth = 1;
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = m_Usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;// should be undefined by the spec

    vkCreateImage(GPUImpl::Get(), &info, nullptr, &m_Handle);

    VkMemoryRequirements req;
    vkGetImageMemoryRequirements(GPUImpl::Get(), m_Handle, &req);

    m_Memory = MemoryAllocator::Alloc(req.size, MemoryType::VRAM);

    vkBindImageMemory(GPUImpl::Get(), m_Handle, m_Memory, 0);
}

void Texture2DImpl::CreateImageView(){
    VkImageViewCreateInfo view_info;
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.pNext = nullptr;
    view_info.flags = 0;
    view_info.image = m_Handle;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = ToVkFormat(m_Format);
    view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.subresourceRange.aspectMask = IsDepthFormat(m_Format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.layerCount = 1;
    view_info.subresourceRange.levelCount = 1;

    vkCreateImageView(GPUImpl::Get(), &view_info, nullptr, &m_View);
}

void Texture2DImpl::DestroyImageView(){
    vkDestroyImageView(GPUImpl::Get(), m_View, nullptr);
}

void Texture2DImpl::DestroyImage(){
    vkDestroyImage(GPUImpl::Get(), m_Handle, nullptr);

    MemoryAllocator::Free(m_Memory);
}


}//namespace Vk::
#ifndef STRAITX_VULKAN_COMMAND_BUFFER_IMPL_HPP
#define STRAITX_VULKAN_COMMAND_BUFFER_IMPL_HPP

#include "core/os/vulkan.hpp"
#include "core/array.hpp"
#include "graphics/api/command_buffer.hpp"
#include "graphics/api/vulkan/gpu_impl.hpp"

namespace Vk{

class CommandPoolImpl: public CommandPool{
private:
    VkCommandPool m_Handle = VK_NULL_HANDLE;
    QueueFamily::Type m_TargetQueueFamily;
    u32 m_TargetQueueFamilyIndex = -1;
public:
    CommandPoolImpl(QueueFamily::Type target_queue_family = QueueFamily::Type::Graphics);

    ~CommandPoolImpl();

    CommandBuffer *Alloc()override;

    void Free(CommandBuffer *buffer)override;

    operator VkCommandPool()const{
        return m_Handle;
    }

    QueueFamily::Type TargetQueueType()const{
        return m_TargetQueueFamily;
    }
};

class CommandBufferImpl: public CommandBuffer{
private:
    CommandPoolImpl *m_Pool = nullptr;
    VkCommandBuffer m_Handle = VK_NULL_HANDLE;
public:
    CommandBufferImpl(CommandPoolImpl *pool);

    ~CommandBufferImpl();

    operator VkCommandBuffer()const{
        return m_Handle;
    }

    CommandPoolImpl* Pool()const{
        return m_Pool;
    }

    void Begin()override;

    void End()override;

    void Reset()override;

    void PipelineBarrier(VkPipelineStageFlags src, VkPipelineStageFlags dst)const;

    void MemoryBarrier(VkPipelineStageFlags src, VkPipelineStageFlags dst, VkAccessFlags src_acces, VkAccessFlags dst_access)const;

    void ImageBarrier(VkPipelineStageFlags src, VkPipelineStageFlags dst, VkAccessFlags src_acces, VkAccessFlags dst_access, VkImageLayout old, VkImageLayout next, VkImage img, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT)const;

    void ChangeLayout(Texture2D *texture, TextureLayout new_layout)override;

    void BeginRenderPass(const RenderPass *rp, const Framebuffer *fb)override;

    void EndRenderPass()override;

    void Bind(const GraphicsPipeline *pipeline)override;

    void Draw(u32 vertices_count)override;

    void SetScissor(s32 x, s32 y, u32 width, u32 height)override;

    void SetViewport(s32 x, s32 y, u32 width, u32 height)override;

    void BindVertexBuffer(const Buffer *buffer)override;

    void BindIndexBuffer(const Buffer *buffer, IndicesType type)override;

    void DrawIndexed(u32 indices_count)override;

    void Copy(const Buffer *src, const Buffer *dst, size_t size, size_t src_offset, size_t dst_offset)override;

    void Copy(const Buffer *src, const Texture2D *dst)override;

    void ClearColor(const Texture2D *texture, const Color &color)override;
};

}//namespace Vk::

#endif//STRAITX_VULKAN_COMMAND_BUFFER_IMPL_HPP
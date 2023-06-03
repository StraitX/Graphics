#ifndef STRAITX_VULKAN_SWAPCHAIN_IMPL_HPP
#define STRAITX_VULKAN_SWAPCHAIN_IMPL_HPP

#include "graphics/api/vulkan/vulkan.hpp"
#include "graphics/api/vulkan/vulkan_surface.hpp"
#include "core/os/window.hpp"
#include "core/math/vector2.hpp"
#include "core/fixed_list.hpp"
#include "graphics/api/swapchain.hpp"
#include "graphics/api/vulkan/queue.hpp"
#include "graphics/api/vulkan/texture_impl.hpp"

namespace Vk{

class SwapchainImpl: public Swapchain{
private:
    // XXX: vulkan can provide us with inf amount of images
	static constexpr size_t s_MaxImages = 5;

    const Window *m_SurfaceWindow = nullptr;
    VulkanSurface  m_Surface;
    VkSwapchainKHR m_Handle = VK_NULL_HANDLE; 

    VkFormat m_Format = VK_FORMAT_B8G8R8A8_UNORM;
    VkColorSpaceKHR m_Colorspace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    Vector2u m_Size; 

    u32 m_CurrentImage = 0;
    u32 m_ImagesCount = 0;

    FixedList<Texture2DImpl, s_MaxImages> m_Images;
    FixedList<Texture2D *, s_MaxImages> m_ImagesPointers;

    QueueFamily::Type m_TargetQueueFamily;
    VkQueue m_TargetQueue = VK_NULL_HANDLE;
    u32 m_TargetQueueIndex = InvalidIndex;
public:
    SwapchainImpl(const Window *window);

    ~SwapchainImpl();

    bool PresentCurrent(const Semaphore &wait_semaphore)override;

    bool AcquireNext(const Semaphore &signal_semaphore, const Fence &signal_fence)override;

    ConstSpan<Texture2D *> Images()const override;

    u32 Current()const override;

    void Recreate()override;

    const Window* PresentTarget() const override {
        return m_SurfaceWindow;
    }
};

}//namespace Vk::

#endif//STRAITX_VULKAN_SWAPCHAIN_IMPL_HPP
#ifndef STRAITX_VULKAN_FENCE_HPP
#define STRAITX_VULKAN_FENCE_HPP

#include "core/os/vulkan.hpp"
#include "core/assert.hpp"
#include "core/noncopyable.hpp"

namespace Vk{

struct Fence: NonCopyable{
    VkFence Handle = VK_NULL_HANDLE;
    
    Fence();

    ~Fence();

	void Wait()const;

	void Reset()const;

	void WaitAndReset()const;
};

}//namespace Vk::

#endif//STRAITX_VULKAN_FENCE_HPP
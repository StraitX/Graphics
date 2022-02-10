#ifndef STRAITX_SWAPCHAIN_HPP
#define STRAITX_SWAPCHAIN_HPP

#include "core/noncopyable.hpp"
#include "core/span.hpp"
#include "core/list.hpp"
#include "graphics/api/semaphore.hpp"
#include "graphics/api/fence.hpp"
#include "graphics/api/texture.hpp"
#include "graphics/api/framebuffer.hpp"
#include "core/os/window.hpp"
#include "core/unique_ptr.hpp"

class Swapchain: public NonCopyable{
public:
    virtual ~Swapchain() = default;

    virtual bool AcquireNext(const Semaphore &signal_semaphore, const Fence &signal_fence = Fence::Null) = 0;

    virtual bool PresentCurrent(const Semaphore &wait_semaphore) = 0;

    //Span may be different after each PresentCurrent call
    virtual ConstSpan<Texture2D *> Images()const = 0;

    virtual u32 Current()const = 0;

    Texture2D *CurrentImage()const{
        return Images()[Current()];
    }

    TextureFormat Format()const{
        return CurrentImage()->Format();
    }

    Vector2u Size()const{
        return CurrentImage()->Size();
    }

    virtual void Recreate() = 0;
public:
    static Swapchain *Create(const Window *window);
};


class FramebufferChain{
private:
    UniquePtr<Swapchain> m_Swapchain{nullptr};
    UniquePtr<RenderPass> m_SwapchainPass{nullptr};
    List<UniquePtr<Framebuffer>> m_Framebuffers;
public:
    FramebufferChain(const Window *window);

    void Recreate();

    const Framebuffer *CurrentFramebuffer()const{
        return m_Framebuffers[m_Swapchain->Current()].Get();
    }

    const RenderPass *Pass()const{
        return m_SwapchainPass.Get();
    }

    void AcquireNext(const Semaphore *signal_semaphore){
        while(!m_Swapchain->AcquireNext(*signal_semaphore))
            Recreate();
    }

    void PresentCurrent(const Semaphore *wait_semaphore){
        if(!m_Swapchain->PresentCurrent(*wait_semaphore))
            Recreate();
    }
private:
    void CreateFramebuffers();
};


#endif//STRAITX_SWAPCHAIN_HPP
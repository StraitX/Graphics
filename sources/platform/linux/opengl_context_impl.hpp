#ifndef STRAITX_LINUX_GLCONTEXT_IMPL_HPP
#define STRAITX_LINUX_GLCONTEXT_IMPL_HPP

#include "platform/linux/window_impl.hpp"
#include "core/types.hpp"

namespace X11{
struct __GLXcontextRec;
}//namespace X11::

namespace Linux{

class OpenGLContextImpl{
private:
    X11::__GLXcontextRec *m_Handle = nullptr;
    unsigned long m_WindowHandle = 0;
public:
    OpenGLContextImpl() = default;

    Result Create(const WindowImpl &window, const Version &version);

    Result CreateLegacy(const WindowImpl &window);

    void Destroy();

    void DestroyLegacy();

    Result MakeCurrent();

    void SwapBuffers();

	void Resize(u32 width, u32 height);

};

} // namespace Linux::

#endif // STRAITX_LINUX_GLCONTEXT_IMPL_HPP
#ifndef STRAITX_OPENGL_CONTEXT_HPP
#define STRAITX_OPENGL_CONTEXT_HPP

#include "platform/platform_detection.hpp"
#include "platform/window.hpp"
#include "platform/noncopyable.hpp"

#ifdef SX_PLATFORM_LINUX
    #include "platform/linux/opengl_context_impl.hpp"
    typedef StraitX::Linux::OpenGLContextImpl PlatformGLContextImpl;
#elif defined(SX_PLATFORM_WINDOWS)
    #include "platform/windows/opengl_context_impl.hpp"
    typedef StraitX::Windows::OpenGLContextImpl PlatformGLContextImpl;
#else
    #error "Your platform does not support OpenGL context"
#endif

namespace StraitX{

class OpenGLContext: public NonCopyable{
private:
    PlatformGLContextImpl m_Impl;
public:

    OpenGLContext() = default;

    Result Create(Window &window, const Version &version);

    Result CreateDummy();

    void Destroy();

    void DestroyDummy();

    Result MakeCurrent();

    void SwapBuffers();
};

sx_inline Result OpenGLContext::Create(Window &window, const Version &version){
    return m_Impl.Create(window.Impl(), version);
}

sx_inline Result OpenGLContext::CreateDummy(){
    return m_Impl.CreateDummy();
}

sx_inline void OpenGLContext::DestroyDummy(){
    m_Impl.DestroyDummy();
}

sx_inline void OpenGLContext::Destroy(){
    m_Impl.Destroy();
}

sx_inline Result OpenGLContext::MakeCurrent(){
    return m_Impl.MakeCurrent();
}

sx_inline void OpenGLContext::SwapBuffers(){
    m_Impl.SwapBuffers();
}

}// namespace StraitX::

#endif // STRAITX_OPENGL_CONTEXT_HPP
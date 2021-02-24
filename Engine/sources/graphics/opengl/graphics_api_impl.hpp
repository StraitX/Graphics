#ifndef STRAITX_OPENGL_GRAPHICS_API_IMPL_HPP
#define STRAITX_OPENGL_GRAPHICS_API_IMPL_HPP

#include "platform/opengl_context.hpp"
#include "graphics/api/graphics_api.hpp"


namespace StraitX{
namespace GL{

struct GraphicsAPIImpl: public GraphicsAPI{
    Version LoadedOpenGLVersion = {};
    const char* VendorName = nullptr;

    static GraphicsAPIImpl Instance;

    virtual Result Initialize()override;

    virtual void Finalize()override;

    virtual u32 GetPhysicalGPUCount()override;

    virtual Result GetPhysicalGPUs(PhysicalGPU *array)override;
};

}// namespace GL::
}// namespace StraitX::

#endif//STRAITX_OPENGL_GRAPHICS_API_IMPL_HPP
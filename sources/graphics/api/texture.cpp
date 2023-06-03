#include "graphics/api/texture.hpp"
#include "graphics/api/graphics_api.hpp"
#include "graphics/api/vulkan/vulkan.hpp"
#include "core/assert.hpp"
#include "core/unique_ptr.hpp"

#ifdef SX_VULKAN_SUPPORTED
    #include "graphics/api/vulkan/texture_impl.hpp"
#endif

bool IsDepthFormat(TextureFormat format){
    if(format == TextureFormat::Depth32
    || format == TextureFormat::Depth24Stencil8)
        return true;
    return false;
}

bool IsStencilFormat(TextureFormat format) {
    if(format == TextureFormat::Depth24Stencil8)
        return true;
    return false;
}

bool IsColorFormat(TextureFormat format){
    if(format == TextureFormat::RGBA8
    || format == TextureFormat::RGB8
    || format == TextureFormat::RGBA16F
    || format == TextureFormat::BGRA8)
        return true;
    return false;
}

size_t GetPixelSize(TextureFormat format){
    switch (format){
    case TextureFormat::Unknown:        return 0;
    case TextureFormat::RGBA8:          return 4;
    case TextureFormat::RGB8:           return 3;
    case TextureFormat::RGBA16F:         return 8;
    case TextureFormat::BGRA8:          return 4;
    case TextureFormat::Depth32:        return 4;
    case TextureFormat::Depth24Stencil8:return 4;
    }
    SX_CORE_ASSERT(false, "Unhandled texture format");
    return 0;
}

Texture2D *Texture2D::Create(u32 width, u32 height, TextureFormat format, TextureUsage usage, TextureLayout initial_layout){
#ifdef SX_VULKAN_SUPPORTED
    if(GraphicsAPI::Backend() == GraphicsAPIBackend::Vulkan)
        return new Vk::Texture2DImpl(width, height, format, usage, initial_layout);
#endif
    return nullptr;
}

Texture2D *Texture2D::Create(const Image &image, TextureUsage usage, TextureLayout initial_layout){
    //SX_CORE_ASSERT(image.Channels() == 3, "Only RGBA images are supported now");

    usage |= TextureUsageBits::TransferDst;

    Texture2D *texture = Create(image.Width(), image.Height(), TextureFormat::RGBA8, usage, initial_layout);

    texture->Copy(image.Data(), Vector2u(image.Width(), image.Height()));

    return texture;
}

Texture2D *Texture2D::Create(StringView filepath, TextureUsage usage, TextureLayout initial_layout){
    Image staging;
    Result res = staging.LoadFromFile(filepath);

    SX_CORE_ASSERT(res == Result::Success, "Can't load image from a file");

    return Create(staging, usage, initial_layout);
}

Texture2D* Texture2D::Black(){
    static UniquePtr<Texture2D> s_BlackTexture = Texture2D::Create(Image(1, 1, Color::Black), TextureUsageBits::Sampled, TextureLayout::ShaderReadOnlyOptimal);
    return s_BlackTexture.Get();
}

Texture2D* Texture2D::White(){
    static UniquePtr<Texture2D> s_WhiteTexture = Texture2D::Create(Image(1, 1, Color::White), TextureUsageBits::Sampled, TextureLayout::ShaderReadOnlyOptimal);
    return s_WhiteTexture.Get();
}

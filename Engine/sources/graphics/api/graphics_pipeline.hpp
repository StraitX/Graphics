#ifndef STRAITX_GRAPHICS_PIPELINE_HPP
#define STRAITX_GRAPHICS_PIPELINE_HPP

#include "core/array_ptr.hpp"
#include "core/noncopyable.hpp"
#include "core/validable.hpp"
#include "graphics/api/shader.hpp"
#include "graphics/api/gpu_buffer.hpp"
#include "graphics/api/logical_gpu.hpp"
#include "graphics/api/render_pass.hpp"
#include "graphics/api/gpu_texture.hpp"
#include "graphics/api/sampler.hpp"

namespace StraitX{


enum class VertexAttribute{
    Int = 0,
    Int2,
    Int3,
    Int4,

    Uint,
    Uint2,
    Uint3,
    Uint4,

    Float,
    Float2,
    Float3,
    Float4
};

enum class ShaderBindingType{
    UniformBuffer = 0,
    Sampler       = 1
};

struct ShaderBinding{
    ShaderBindingType Type;
    Shader::Types VisibleShaders;

    constexpr ShaderBinding(ShaderBindingType type, Shader::Types visible_shaders):
        Type(type),
        VisibleShaders(visible_shaders)
    {}
};

//TODO: Take value from docs
constexpr size_t MaxShaderBindings = 8;

enum class PrimitivesTopology{
    Points = 0,
    Lines,
    LinesStrip,
    Triangles,
    TrianglesStrip
};

enum class RasterizationMode{
    Fill = 0,
    Lines,
    Points
};

enum class BlendFunction{
    Add,
};

enum class BlendFactor{
    Zero,
    One,
    OneMinusSrcAlpha,
    OneMinusDstAlpha,
    SrcAlpha,
    DstAlpha 
};

struct Viewport{
    s32 x;
    s32 y;
    u32 Width;
    u32 Height;
};

struct GraphicsPipelineProperties{
    ArrayPtr<const Shader* const> Shaders;
    ArrayPtr<const VertexAttribute> VertexAttributes;
    PrimitivesTopology Topology;
    Viewport FramebufferViewport;
    RasterizationMode Rasterization;
    BlendFunction BlendFunc;
    BlendFactor SrcBlendFactor;
    BlendFactor DstBlendFactor;
    const RenderPass *Pass;
    ArrayPtr<const ShaderBinding> ShaderBindings;
};
//OpenGL said that
constexpr size_t MaxVertexAttributes = 8;

class GraphicsAPILoader;

class GraphicsPipeline: public Validable, public NonCopyable{
public:
    struct VTable{
        using NewProc    = GraphicsPipeline * (*)(LogicalGPU &owner, const GraphicsPipelineProperties &props);
        using DeleteProc = void (*)(GraphicsPipeline *);

        NewProc    New    = nullptr;
        DeleteProc Delete = nullptr;
    };
private:
    static VTable s_VTable;

    friend class GraphicsAPILoader;
protected:
    static u32 s_VertexAttributeSizeTable[];
public:
    GraphicsPipeline(const GraphicsPipelineProperties &props);

    virtual ~GraphicsPipeline() = default;

    sx_inline static GraphicsPipeline *New(const GraphicsPipelineProperties &props);

    sx_inline static void Delete(GraphicsPipeline *pipeline);

    virtual void Bind(size_t index, const GPUBuffer &uniform_buffer) = 0;

    virtual void Bind(size_t index, const GPUTexture &texture, const Sampler &sampler) = 0;

    static size_t CalculateStride(const ArrayPtr<const VertexAttribute> &attributes);

};

sx_inline GraphicsPipeline *GraphicsPipeline::New(const GraphicsPipelineProperties &props){
    return s_VTable.New(LogicalGPU::Instance(), props);
}

sx_inline void GraphicsPipeline::Delete(GraphicsPipeline *pipeline){
    s_VTable.Delete(pipeline);
}

}//namespace StraitX::

#endif//STRAITX_GRAPHICS_PIPELINE_HPP
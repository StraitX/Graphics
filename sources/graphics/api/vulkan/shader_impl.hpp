#ifndef STRAITX_VULKAN_SHADER_IMPL_HPP
#define STRAITX_VULKAN_SHADER_IMPL_HPP

#include "graphics/api/vulkan/vulkan.hpp"
#include "graphics/api/shader.hpp"

namespace Vk{

VkShaderStageFlagBits ToVkShaderStage(ShaderStageBits::Value stage);

class ShaderImpl: public Shader{
private:
    VkShaderModule m_Handle = VK_NULL_HANDLE;
    ShaderLang m_Lang;
    ShaderStageBits::Value m_Stage;
public:
    ShaderImpl(ShaderStageBits::Value stage, StringView sources, ShaderCompileOptions opts);

    ~ShaderImpl();

    operator VkShaderModule()const{
        return m_Handle;
    }

    ShaderLang Lang()const override;

    ShaderStageBits::Value Stage()const override;
};

}//namespace Vk::

#endif//STRAITX_VULKAN_SHADER_IMPL_HPP
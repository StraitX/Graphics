#ifndef STRAITX_GRAPHICS_CONTEXT_HPP
#define STRAITX_GRAPHICS_CONTEXT_HPP

#include "platform/window.hpp"
#include "graphics/api/render_pass.hpp"
#include "graphics/api/framebuffer.hpp"

namespace StraitX{

class GraphicsAPI{
public:
	enum Value{
		None = 0,
		Vulkan = 1,
		OpenGL = 2
	};
private:
	Value m_Value;
public:
	GraphicsAPI(Value value = Value::None):
		m_Value(value)
	{}

	const char *Name()const;

	bool operator==(GraphicsAPI other){
		return m_Value == other.m_Value;
	}

	bool operator!=(GraphicsAPI other){
		return !(*this == other);
	}

	operator Value()const{
		return m_Value;
	}
};

class GraphicsContext: public NonCopyable{
private:
	static GraphicsContext *s_Instance;
	static GraphicsAPI s_CurrentAPI;

	friend class GraphicsAPILoader;
	friend class Engine;
public:
	GraphicsAPI API()const;
private:
	virtual Result Initialize(const Window &window) = 0;

	virtual void Finalize() = 0;
public:
	//virtual void ScheduleCmdBuffer(CmdBuffer cmd_buffer) = 0;

	virtual void BeginFrame() = 0;

	//calls ExecuteCmdBuffers()
	virtual void EndFrame() = 0;

	//virtual void ExecuteScheduledCmdBuffers() = 0;

	virtual void SwapBuffers() = 0;

	virtual const Framebuffer *CurrentFramebuffer() = 0;

	virtual const RenderPass *FramebufferPass() = 0;
public:
	static GraphicsContext &Get();
};

SX_INLINE GraphicsAPI GraphicsContext::API()const{
	return s_CurrentAPI;
}

SX_INLINE GraphicsContext &GraphicsContext::Get(){
	SX_CORE_ASSERT(s_Instance, "GraphicsContext was not initialized");

	return *s_Instance;
}

}//namespace StraitX::

#endif //STRAITX_GRAPHICS_CONTEXT_HPP
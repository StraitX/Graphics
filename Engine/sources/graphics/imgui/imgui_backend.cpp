#include "graphics/imgui/imgui_backend.hpp"
#include "graphics/imgui/imgui.hpp"
#include "graphics/api/cpu_texture.hpp"
#include "graphics/api/dma.hpp"
#include "core/string.hpp"
#include "servers/display_server.hpp"
#include "platform/keyboard.hpp"

namespace StraitX{

ShaderBinding ImGuiBackend::s_Bindings[2] = {
	{0, 1, ShaderBindingType::Texture, Shader::Fragment},
	{1, 1, ShaderBindingType::UniformBuffer, Shader::Vertex},
};

VertexAttribute ImGuiBackend::s_Attributes[3]={
	VertexAttribute::Float32x2,
	VertexAttribute::Float32x2,
	VertexAttribute::UNorm8x4,
};

const char *s_VertexShader = R"(
#version 440 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;

layout(binding = 1) uniform uPushConstant { vec2 uScale; vec2 uTranslate; } pc;

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out struct { vec4 Color; vec2 UV; } Out;

void main()
{
	Out.Color = aColor;
    Out.UV = aUV;
    gl_Position = vec4(aPos * pc.uScale + pc.uTranslate, 0, 1);
	gl_Position.y *= -1.0;
}
)";

const char *s_FragmentShader = R"(
#version 440 core
layout(location = 0) out vec4 fColor;
layout(binding = 0) uniform sampler2D sTexture;
layout(location = 0) in struct { vec4 Color; vec2 UV; } In;
void main()
{
    fColor = In.Color * texture(sTexture, In.UV.st);
}
)";

void ImGuiBackend::Initialize(){
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsDark();
	ImGui::GetStyle().WindowRounding = 8;

    io.BackendPlatformName = "StraitX ImGui Backend";
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;  

	io.KeyMap[ImGuiKey_Tab] = (int)Key::Tab;
    io.KeyMap[ImGuiKey_LeftArrow] = (int)Key::Left;
    io.KeyMap[ImGuiKey_RightArrow] = (int)Key::Right;
    io.KeyMap[ImGuiKey_UpArrow] = (int)Key::Up;
    io.KeyMap[ImGuiKey_DownArrow] = (int)Key::Down;
    io.KeyMap[ImGuiKey_PageUp] = (int)Key::PageUp;
    io.KeyMap[ImGuiKey_PageDown] = (int)Key::PageDown;
    io.KeyMap[ImGuiKey_Home] = (int)Key::Home;
    io.KeyMap[ImGuiKey_End] = (int)Key::End;
    io.KeyMap[ImGuiKey_Insert] = (int)Key::Insert;
    io.KeyMap[ImGuiKey_Delete] = (int)Key::Delete;
    io.KeyMap[ImGuiKey_Backspace] = (int)Key::Backspace;
    io.KeyMap[ImGuiKey_Space] = (int)Key::Space;
    io.KeyMap[ImGuiKey_Enter] = (int)Key::Enter;
    io.KeyMap[ImGuiKey_Escape] = (int)Key::Escape;
    io.KeyMap[ImGuiKey_KeyPadEnter] = (int)Key::KeypadEnter;
    io.KeyMap[ImGuiKey_A] = (int)Key::A;
    io.KeyMap[ImGuiKey_C] = (int)Key::C;
    io.KeyMap[ImGuiKey_V] = (int)Key::V;
    io.KeyMap[ImGuiKey_X] = (int)Key::X;
    io.KeyMap[ImGuiKey_Y] = (int)Key::Y;
    io.KeyMap[ImGuiKey_Z] = (int)Key::Z;

	{
		unsigned char *pixels = nullptr;
		int width = 0;
		int height = 0;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		CPUTexture staging_font_texture;
		staging_font_texture.New(width, height, TextureFormat::RGBA8, pixels);

		m_ImGuiFont.New(width, height, TextureFormat::RGBA8, TextureUsageBits((int)TextureUsageBits::Sampled | (int)TextureUsageBits::TransferDst));
		DMA::ChangeLayout(m_ImGuiFont, TextureLayout::TransferDstOptimal);
		DMA::Copy(staging_font_texture, m_ImGuiFont);
		DMA::ChangeLayout(m_ImGuiFont, TextureLayout::ShaderReadOnlyOptimal);

		io.Fonts->SetTexID(ImTextureID(m_ImGuiFont.Handle().U64));
	}

	{
		m_DescriptorSetLayout = new DescriptorSetLayout(s_Bindings);

		m_DescriptorSetPool = DescriptorSetPool::New(m_DescriptorSetLayout, 1);

		m_Set = m_DescriptorSetPool->AllocateSet();
	}

	{
		m_Shaders[0] = Shader::New(Shader::Vertex, Shader::Lang::GLSL, (const u8*)s_VertexShader, String::Length(s_VertexShader));		
		m_Shaders[1] = Shader::New(Shader::Fragment, Shader::Lang::GLSL, (const u8*)s_FragmentShader, String::Length(s_FragmentShader));	

		SX_ASSERT(m_Shaders[0]->IsValid());
		SX_ASSERT(m_Shaders[1]->IsValid());

		GraphicsPipelineProperties props;
		props.Shaders = m_Shaders;
		props.VertexAttributes = s_Attributes;
		props.PrimitivesTopology = PrimitivesTopology::Triangles;
		props.RasterizationMode = RasterizationMode::Fill;
		props.BlendFunction = BlendFunction::Add;
		props.DepthFunction = DepthFunction::Always;
		props.SrcBlendFactor = BlendFactor::SrcAlpha;
		props.DstBlendFactor = BlendFactor::OneMinusSrcAlpha;
		props.Pass = GraphicsContext::Get().FramebufferPass();
		props.DescriptorSetLayout = m_DescriptorSetLayout; 	

		m_Pipeline = GraphicsPipeline::New(props);

		SX_ASSERT(m_Pipeline->IsValid());
 	}

	m_UniformBuffer.New(sizeof(Uniform), GPUMemoryType::DynamicVRAM, GPUBuffer::TransferDestination | GPUBuffer::UniformBuffer);
	m_Set->UpdateTextureBinding(0, 0, m_ImGuiFont);
	m_Set->UpdateUniformBinding(1, 0, m_UniformBuffer);
}

void ImGuiBackend::BeginFrame(float dt){
	ImGuiIO& io = ImGui::GetIO();

	auto window_size = DisplayServer::Window.Size();

	io.DisplaySize = ImVec2((float)window_size.width, (float)window_size.height);
	io.DisplayFramebufferScale = ImVec2(1, 1);
	io.DeltaTime = dt;

	auto mouse_pos = Mouse::RelativePosition(DisplayServer::Window);

	io.MousePos = ImVec2((float)mouse_pos.x, (float)mouse_pos.y);

	if(Mouse::IsButtonPressed(Mouse::Left))
		io.MouseDown[ImGuiMouseButton_Left] = true;
	if(Mouse::IsButtonPressed(Mouse::Right))
		io.MouseDown[ImGuiMouseButton_Right] = true;
	if(Mouse::IsButtonPressed(Mouse::Middle))
		io.MouseDown[ImGuiMouseButton_Middle] = true;

	ImGui::NewFrame();
}

void ImGuiBackend::EndFrame(){
	ImGui::Render();
	const ImDrawData *data = ImGui::GetDrawData();

	auto window_size = DisplayServer::Window.Size();
	ImVec2 clip_off = data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = data->FramebufferScale;

	Uniform uniform;
	uniform.u_Scale.x = 2.0f / data->DisplaySize.x;
	uniform.u_Scale.y = 2.0f / data->DisplaySize.y;
	uniform.u_Translate.x = -1.0f - data->DisplayPos.x * uniform.u_Scale.x;
	uniform.u_Translate.y = -1.0f - data->DisplayPos.x * uniform.u_Scale.y;

	DMA::Copy(&uniform, m_UniformBuffer);


	for(int i = 0; i<data->CmdListsCount; i++){

		auto cmd_list = data->CmdLists[i];

		m_VertexBuffer.New(cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), GPUMemoryType::DynamicVRAM, GPUBuffer::VertexBuffer | GPUBuffer::TransferDestination);
		m_IndexBuffer.New(cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), GPUMemoryType::DynamicVRAM, GPUBuffer::IndexBuffer | GPUBuffer::TransferDestination);

		DMA::Copy(cmd_list->VtxBuffer.Data, m_VertexBuffer);
		DMA::Copy(cmd_list->IdxBuffer.Data, m_IndexBuffer);

		for(const auto &cmd: cmd_list->CmdBuffer){

			m_Set->UpdateTextureBinding(0, 0, m_ImGuiFont);
			m_Set->UpdateUniformBinding(1, 0, m_UniformBuffer);
			m_CmdBuffer.BindPipeline(m_Pipeline);
			m_CmdBuffer.BindDescriptorSet(m_Set);
			m_CmdBuffer.SetViewport(window_size.width, window_size.height, 0, 0);

			ImVec4 clip_rect;
			clip_rect.x = (cmd.ClipRect.x - clip_off.x) * clip_scale.x;
			clip_rect.y = (cmd.ClipRect.y - clip_off.y) * clip_scale.y;
			clip_rect.z = (cmd.ClipRect.z - clip_off.x) * clip_scale.x;
			clip_rect.w = (cmd.ClipRect.w - clip_off.y) * clip_scale.y;

			if (clip_rect.x < 0.0f)
				clip_rect.x = 0.0f;
			if (clip_rect.y < 0.0f)
				clip_rect.y = 0.0f;
			m_CmdBuffer.BindVertexBuffer(m_VertexBuffer);
			m_CmdBuffer.BindIndexBuffer(m_IndexBuffer, IndicesType::Uint16);

			Vector2f offset_original(clip_rect.x, clip_rect.y);
			Vector2f extent_original(clip_rect.z - clip_rect.x, clip_rect.w - clip_rect.y);

			Vector2f offset(offset_original.x, window_size.height - extent_original.y - offset_original.y);
			Vector2f extent = extent_original;

			m_CmdBuffer.SetScissors(extent.x, extent.y, offset.x, offset.y);

			m_CmdBuffer.BeginRenderPass(GraphicsContext::Get().FramebufferPass(), GraphicsContext::Get().CurrentFramebuffer());
				m_CmdBuffer.DrawIndexed(cmd.ElemCount, cmd.IdxOffset);
			m_CmdBuffer.EndRenderPass();

			GraphicsContext::Get().ExecuteCmdBuffer(m_CmdBuffer);
			m_CmdBuffer.Reset();
		}


		m_VertexBuffer.Delete();
		m_IndexBuffer.Delete();
	}

	ImGuiIO &io = ImGui::GetIO();

	for(int i = 0; i<lengthof(io.MouseDown); i++)
		io.MouseDown[i] = false;
		
}

void ImGuiBackend::OnEvent(const Event &e){
	ImGuiIO &io = ImGui::GetIO();

	switch(e.Type){
	case EventType::KeyPress:
		io.KeyMap[(size_t)e.KeyPress.KeyCode] = true;
		break;
	case EventType::KeyRelease:
		io.KeyMap[(size_t)e.KeyRelease.KeyCode] = false;
		break;
	default:
		(void)0;
	}
}

void ImGuiBackend::Finalize(){
	m_UniformBuffer.Delete();

	m_DescriptorSetPool->FreeSet(m_Set);

	DescriptorSetPool::Delete(m_DescriptorSetPool);

	delete m_DescriptorSetLayout;

	Shader::Delete((Shader *)m_Shaders[0]);
	Shader::Delete((Shader *)m_Shaders[1]);

	GraphicsPipeline::Delete(m_Pipeline);
	m_ImGuiFont.Delete();
}

}//namespace StraitX::
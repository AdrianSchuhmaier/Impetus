#include "pch.h"
#include "Renderer.h"

#include "Vulkan/Context.h"
#include "Vulkan/Frame.h"
#include "Vulkan/RenderPass.h"
#include "Vulkan/ShaderLibrary.h"

#include "Shader.h"

namespace Prism {

	// ========= Frame System ===============================================
	constexpr uint8_t FRAME_COUNT = 1;
	struct {
		uint32_t currentFrame = 0;
		Vulkan::Frame* frames;

		Vulkan::Frame& get() { return frames[currentFrame]; }
		void next() { currentFrame = (currentFrame + 1) % FRAME_COUNT; }
	} g_Frame;
	// ======================================================================

	bool minimized = false;

	std::vector<std::weak_ptr<RenderObject>> Renderer::renderObjects{};
	std::unique_ptr<Shader> shader;
	Vulkan::RenderPass* renderPass;

	void Renderer::Init(const Window* window)
	{
		Vulkan::Context::Init(window);

		g_Frame.frames = new Vulkan::Frame[FRAME_COUNT];
		shader = std::make_unique<Shader>("assets/shader/shader.glsl");

		renderPass = &Vulkan::RenderPass::GetDefaultPass();

		renderPass->SetClearValue(vk::ClearColorValue(std::array<float, 4>({0.2f, 0.2f, 0.2f, 1.0f})));
	}

	void Renderer::Shutdown()
	{
		ShaderLibrary::CleanUp();

		delete[] g_Frame.frames;
		Vulkan::RenderPass::CleanUpStatic();
		Vulkan::Context::CleanUp();
	}

	void Renderer::Prepare()
	{
		g_Frame.next();
	}

	void Renderer::Render()
	{
		if (!minimized)
		{
			g_Frame.get().Begin();
			renderPass->Begin(
				g_Frame.get().GetCommandBuffer(),
				g_Frame.get().GetFramebuffer(),
				vk::SubpassContents::eInline);

			ShaderLibrary::PipelineOf(*shader.get())->Bind(g_Frame.get().GetCommandBuffer());
			g_Frame.get().GetCommandBuffer().draw(3, 1, 0, 0);

			renderPass->End(g_Frame.get().GetCommandBuffer());
			g_Frame.get().End();
		}
		else
		{
			PR_CORE_WARN("Rendering paused");
		}
	}

	void Renderer::Register(const std::shared_ptr<RenderObject>& renderObject)
	{
		renderObjects.emplace_back(renderObject);
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		Vulkan::Context::Resize(width, height);
	}
}
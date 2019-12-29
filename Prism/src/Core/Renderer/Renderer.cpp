#include "pch.h"
#include "Renderer.h"

#include "Vulkan/Context.h"
#include "Vulkan/Frame.h"
#include "Vulkan/RenderPass.h"
#include "Vulkan/ShaderLibrary.h"

#include "Components/RenderComponent.h"

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

	std::set<uint32_t> Renderer::freeIndices = {};
	std::vector<RenderObject> Renderer::renderObjects = {};

	Vulkan::RenderPass* renderPass;

	void Renderer::Init(const Window* window)
	{
		Vulkan::Context::Init(window);

		g_Frame.frames = new Vulkan::Frame[FRAME_COUNT];

		renderPass = &Vulkan::RenderPass::GetDefaultPass();
		renderPass->SetClearValue(vk::ClearColorValue(std::array<float, 4>({ 0.2f, 0.2f, 0.2f, 1.0f })));
	}

	void Renderer::Shutdown()
	{
		renderObjects.clear();
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
		for (const auto& object : renderObjects)
		{
			g_Frame.get().Begin();
			renderPass->Begin(
				g_Frame.get().GetCommandBuffer(),
				g_Frame.get().GetFramebuffer(),
				vk::SubpassContents::eInline);

			ShaderLibrary::PipelineOf(object.material.shader)->Bind(g_Frame.get().GetCommandBuffer());
			g_Frame.get().GetCommandBuffer().draw(3, 1, 0, 0);

			renderPass->End(g_Frame.get().GetCommandBuffer());
			g_Frame.get().End();
		}
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		Vulkan::Context::Resize(width, height);
	}

	RenderObjectHandle Renderer::Register(const RenderComponent& component)
	{
		RenderObjectHandle index;

		if (freeIndices.empty())
		{
			// construct new RenderObject at back of the vector
			index = renderObjects.size();
			renderObjects.emplace_back(component.mesh, *component.material.get());
		}
		else
		{
			// replace mesh & material of the first unused RenderObject
			index = *freeIndices.begin();
			freeIndices.erase(freeIndices.begin());

			renderObjects[index].mesh = component.mesh;
			renderObjects[index].material = *component.material.get();
		}

		return index;
	}

	void Renderer::Unregister(RenderObjectHandle index)
	{
		freeIndices.insert(index);

		renderObjects[index].mesh = nullptr;
		renderObjects[index].material.~Material();
	}
}
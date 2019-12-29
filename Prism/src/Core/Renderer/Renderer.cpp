#include "pch.h"
#include "Renderer.h"

#include "Vulkan/Context.h"
#include "Vulkan/Frame.h"
#include "Vulkan/RenderPass.h"
#include "Vulkan/ShaderLibrary.h"
#include "Vulkan/MemoryManager.h"

#include "Components/RenderComponent.h"

#include "Material.h"

namespace Prism {

	// ========= Frame System ===============================================
	constexpr uint8_t FRAME_COUNT = 1;

	uint32_t current = 0;
	Vulkan::Frame* frames;

	Vulkan::Frame& currentFrame() { return frames[current]; }
	void nextFrame() { current = (current + 1) % FRAME_COUNT; }
	// ======================================================================

	Vulkan::RenderPass* renderPass;

	// ========= RenderObject System ========================================
	std::set<uint32_t> Renderer::freeIndices = {};
	std::vector<RenderObject> Renderer::renderObjects = {};
	// ======================================================================

	// ========= Forward declarations =======================================
	void drawMesh(Mesh* mesh);
	// ======================================================================


	void Renderer::Init(const Window* window)
	{
		Vulkan::Context::Init(window);
		Vulkan::MemoryManager::Init();

		frames = new Vulkan::Frame[FRAME_COUNT];

		renderPass = &Vulkan::RenderPass::GetDefaultPass();
		renderPass->SetClearValue(vk::ClearColorValue(std::array<float, 4>({ 0.2f, 0.2f, 0.2f, 1.0f })));
	}

	void Renderer::Shutdown()
	{
		Vulkan::Context::GetDevice().waitIdle();

		renderObjects.clear();
		ShaderLibrary::CleanUp();

		delete[] frames;
		Vulkan::RenderPass::CleanUpStatic();
		Vulkan::MemoryManager::CleanUp();
		Vulkan::Context::CleanUp();
	}

	void Renderer::Prepare()
	{
		nextFrame();
	}

	void Renderer::Render()
	{
		currentFrame().Begin();

		renderPass->Begin(
			currentFrame().GetCommandBuffer(),
			currentFrame().GetFramebuffer(),
			vk::SubpassContents::eInline);

		for (const auto& object : renderObjects)
		{
			ShaderLibrary::PipelineOf(object.material.shader)->Bind(currentFrame().GetCommandBuffer());
			drawMesh(object.mesh.get());
		}

		renderPass->End(currentFrame().GetCommandBuffer());
		currentFrame().End();
	}

	void drawMesh(Mesh* mesh)
	{
		const auto& vb = static_cast<Vulkan::VertexBuffer*>(mesh->vertexBuffer.get());

		vk::Buffer vertexBuffers[] = { vb->GetBuffer().bufferHandle };
		vk::DeviceSize offsets[] = { 0 };
		currentFrame().GetCommandBuffer().bindVertexBuffers(0, 1, vertexBuffers, offsets);

		currentFrame().GetCommandBuffer().draw(mesh->vertexCount, 1, 0, 0);
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
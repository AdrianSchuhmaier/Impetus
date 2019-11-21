#include "pch.h"
#include "Renderer.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanRenderer.h"

namespace Prism {

	std::vector<std::weak_ptr<RenderObject>> Renderer::renderObjects{};

	void Renderer::Init(const Window* window)
	{
		VulkanContext::Init(window);
		VulkanRenderer::Init(window);
	}

	void Renderer::Shutdown()
	{
		VulkanRenderer::Shutdown();
		VulkanContext::CleanUp();
	}

	void Renderer::Prepare()
	{
	}

	void Renderer::Render()
	{

	}

	void Renderer::Register(const std::shared_ptr<RenderObject>& renderObject)
	{
		renderObjects.emplace_back(renderObject);
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		VulkanContext::Resize(width, height);
	}
}
#include "pch.h"
#include "Frame.h"

#include "Context.h"
#include "Core/Renderer/Defaults/VulkanDefaults.h"

namespace Prism::Vulkan {

	std::vector<vk::Fence> g_ImagesInFlight;

	Frame::Frame()
	{
		const auto& device = Context::GetDevice();
		m_ImageAvailable = device.createSemaphoreUnique({});
		m_RenderFinished = device.createSemaphoreUnique({});
		m_InFlightFence = device.createFenceUnique({ vk::FenceCreateFlagBits::eSignaled });
		m_CommandPool = CommandPool(Context::GetGraphicsQueue().familyIndex);
		m_CommandBuffer = m_CommandPool.AllocateBuffer(vk::CommandBufferLevel::ePrimary);

		if (g_ImagesInFlight.size() < 1)
			g_ImagesInFlight.resize(Context::GetSwapchain().images.size(), nullptr);
	}

	bool Frame::Begin()
	{
		// if frame still in flight, wait
		Context::GetDevice().waitForFences(m_InFlightFence.get(), true, std::numeric_limits<uint64_t>::max());

		m_CommandPool.Reset();

		// get image index
		auto& [result, i] = Context::GetDevice().acquireNextImageKHR(Context::GetSwapchain().swapchain,
			std::numeric_limits<uint64_t>::max(), m_ImageAvailable.get(), nullptr);

		if (g_ImagesInFlight[i])
			Context::GetDevice().waitForFences(g_ImagesInFlight[i], true, std::numeric_limits<uint64_t>::max());
		g_ImagesInFlight[i] = m_InFlightFence.get();

		// check if image was acquired
		if (result == vk::Result::eErrorOutOfDateKHR)
			return false; // surface should be resized
		else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
		{
			PR_CORE_ASSERT(false, "Could not acquire Swapchain image"); // should not happen
			return false;
		}

		m_ImageAcquired = i;

		// begin CommandBuffer
		m_CommandBuffer.begin({ vk::CommandBufferUsageFlagBits::eSimultaneousUse });
	}

	void Frame::End()
	{
		m_CommandBuffer.end();


		// configure semaphore waits
		const vk::Semaphore waitSemaphores[] = { m_ImageAvailable.get() };
		const vk::Semaphore signalSemaphores[] = { m_RenderFinished.get() };
		const vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

		// submit commandbuffer
		vk::SubmitInfo submitInfo(
			1, waitSemaphores,
			waitStages,
			1, &m_CommandBuffer,
			1, signalSemaphores);

		Context::GetDevice().resetFences(m_InFlightFence.get());
		Context::GetGraphicsQueue().queue.submit(submitInfo, m_InFlightFence.get());

		// present
		vk::PresentInfoKHR presentInfo(
			1, signalSemaphores,
			1, &Context::GetSwapchain().swapchain,
			&m_ImageAcquired);

		Context::GetGraphicsQueue().queue.presentKHR(presentInfo);
	}

	vk::Framebuffer Frame::GetFramebuffer()
	{
		return Context::GetSwapchain().framebuffers[m_ImageAcquired];
	}
}
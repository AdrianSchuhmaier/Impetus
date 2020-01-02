#pragma once

#include "vulkan/vulkan.hpp"
#include "CommandPool.h"
#include "DescriptorPool.h"

namespace Prism::Vulkan {
	class Frame
	{
	public:
		Frame();

		bool Begin();
		void End();

		vk::CommandBuffer GetCommandBuffer() { return m_CommandBuffer; }
		vk::Framebuffer GetFramebuffer();

	private:
		uint32_t m_ImageAcquired;
		vk::UniqueSemaphore m_ImageAvailable, m_RenderFinished;
		vk::UniqueFence m_InFlightFence;

		CommandPool m_CommandPool;
		vk::CommandBuffer m_CommandBuffer;
	};
}
#include "pch.h"
#include "CommandPool.h"
#include "Context.h"


namespace Prism::Vulkan {
	CommandPool::CommandPool(uint32_t queueFamilyIndex)
	{
		m_Pool = Context::GetDevice().createCommandPool(
			vk::CommandPoolCreateInfo({}, queueFamilyIndex));
	}

	CommandPool::CommandPool(CommandPool&& other)
		: m_Pool(other.m_Pool)
	{
		other.m_Pool = nullptr;
	}

	CommandPool& CommandPool::operator=(CommandPool&& other)
	{
		if (this != &other)
		{
			if (m_Pool)
				Context::GetDevice().destroyCommandPool(m_Pool);

			m_Pool = other.m_Pool;

			other.m_Pool = nullptr;
		}
		return *this;
	}

	CommandPool::~CommandPool()
	{
		if (m_Pool)	Context::GetDevice().destroyCommandPool(m_Pool);
		m_Pool = nullptr; // to allow explicit destructor call for static members before Context::CleanUp
	}

	void CommandPool::SubmitSingleUse(vk::Queue queue, Commands commands)
	{
		PR_CORE_ASSERT(m_Pool, "No CommandPool to allocate from");
		vk::CommandBuffer buffer = Context::GetDevice().allocateCommandBuffers(
			vk::CommandBufferAllocateInfo(m_Pool, vk::CommandBufferLevel::ePrimary, 1))[0];
		buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
		commands(buffer);
		buffer.end();
		vk::SubmitInfo submitInfo;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &buffer;
		queue.submit(submitInfo, nullptr);
		queue.waitIdle();
		Context::GetDevice().freeCommandBuffers(m_Pool, buffer);
	}

	void CommandPool::Reset()
	{
		Context::GetDevice().resetCommandPool(m_Pool, vk::CommandPoolResetFlagBits::eReleaseResources);
	}

	vk::CommandBuffer CommandPool::AllocateBuffer(vk::CommandBufferLevel level) const
	{
		auto info = vk::CommandBufferAllocateInfo(m_Pool, level, 1);
		return Context::GetDevice().allocateCommandBuffers(info)[0];
	}
}

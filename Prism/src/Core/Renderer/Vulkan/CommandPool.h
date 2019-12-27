#pragma once
#include "vulkan/vulkan.hpp"
#include <functional>

namespace Prism::Vulkan {

	using Commands = std::function<void(vk::CommandBuffer)>;

	class CommandPool
	{
	public:

		void SubmitSingleUse(vk::Queue queue, Commands commands);

		void Reset();

		vk::CommandBuffer AllocateBuffer(vk::CommandBufferLevel level) const;


		// Constructors
		CommandPool() = default;
		CommandPool(uint32_t queueFamilyIndex);
		CommandPool(CommandPool&& other);
		CommandPool& operator=(CommandPool&& other);

		CommandPool(const CommandPool&) = delete;
		CommandPool& operator=(const CommandPool&) = delete;

		~CommandPool();

	private:
		vk::CommandPool m_Pool;
	};

}

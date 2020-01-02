#pragma once

#include "vulkan/vulkan.hpp"
#include "Context.h"
#include "Buffer.h"

namespace Prism::Vulkan {

	class DescriptorSet
	{
	public:
		void Update(UniformBuffer* buffer);
		vk::DescriptorSet GetHandle() const { return m_Set; }

	private:
		friend class DescriptorPool;
		vk::DescriptorSet m_Set;
		vk::DescriptorSetLayout m_Layout;
	};

	class DescriptorPool
	{
	public:
		DescriptorPool() = default;
		DescriptorPool(
			vk::DescriptorType type,
			uint32_t descriptorCount,
			uint32_t maxDescriptorSets);
		DescriptorPool(DescriptorPool&& other);
		DescriptorPool& operator=(DescriptorPool&& other);
		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;

		DescriptorSet AllocateDescriptorSet(vk::DescriptorSetLayout layout);

		~DescriptorPool();

	private:
		vk::DescriptorPool m_Pool = nullptr;
	};
}
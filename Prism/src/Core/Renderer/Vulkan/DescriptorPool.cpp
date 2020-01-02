#include "pch.h"
#include "DescriptorPool.h"

namespace Prism::Vulkan {

	DescriptorPool::DescriptorPool(vk::DescriptorType type, uint32_t descriptorCount, uint32_t maxDescriptorSets)
	{
		vk::DescriptorPoolSize sizeInfo(type, descriptorCount);
		vk::DescriptorPoolCreateInfo createInfo(vk::DescriptorPoolCreateFlags(), maxDescriptorSets, 1, &sizeInfo);
		m_Pool = Context::GetDevice().createDescriptorPool(createInfo);
	}

	DescriptorSet DescriptorPool::AllocateDescriptorSet(vk::DescriptorSetLayout layout)
	{
		DescriptorSet result;
		vk::DescriptorSetAllocateInfo allocInfo(m_Pool, 1, &layout);
		result.m_Set = Context::GetDevice().allocateDescriptorSets(allocInfo)[0];
		result.m_Layout = layout;
		return result;
	}

	DescriptorPool::DescriptorPool(DescriptorPool&& other)
		: m_Pool(other.m_Pool)
	{
		other.m_Pool = nullptr;
	}

	DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other)
	{
		if (this != &other)
		{
			if (m_Pool)
				Context::GetDevice().destroyDescriptorPool(m_Pool);

			m_Pool = other.m_Pool;

			other.m_Pool = nullptr;
		}
		return *this;
	}

	DescriptorPool::~DescriptorPool()
	{
		if (m_Pool) Context::GetDevice().destroyDescriptorPool(m_Pool);
		m_Pool = nullptr;
	}

	void DescriptorSet::Update(UniformBuffer* buffer)
	{
		vk::DescriptorBufferInfo bufferInfo(buffer->GetBuffer().bufferHandle, 0, buffer->GetSize());
		vk::WriteDescriptorSet writeSet(m_Set, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo);
		Context::GetDevice().updateDescriptorSets(1, &writeSet, 0, nullptr);
	}
}
#include "pch.h"
#include "DescriptorPool.h"
#include "Core/Renderer/Defaults/VulkanDefaults.h"

namespace Prism::Vulkan {

	DescriptorPool::DescriptorPool(const std::vector<vk::DescriptorPoolSize>& sizes, uint32_t maxDescriptorSets)
	{
		vk::DescriptorPoolCreateInfo createInfo(vk::DescriptorPoolCreateFlags(), maxDescriptorSets,
			static_cast<uint32_t>(sizes.size()), sizes.data());
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

	void DescriptorSet::Update(Texture2D* texture)
	{
		vk::DescriptorImageInfo imageInfo(Defaults::GetDefaultTextureSampler(), texture->GetImage().imageView, vk::ImageLayout::eShaderReadOnlyOptimal);
		vk::WriteDescriptorSet writeSet(m_Set, 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &imageInfo);
		Context::GetDevice().updateDescriptorSets(1, &writeSet, 0, nullptr);
	}
}
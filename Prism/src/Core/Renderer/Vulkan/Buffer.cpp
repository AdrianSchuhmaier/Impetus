#include "pch.h"
#include "Buffer.h"
#include "MemoryManager.h"

namespace Prism::Vulkan {
	Buffer::~Buffer()
	{
		if (allocation)
			MemoryManager::FreeBuffer(this);
	}

	vk::Format generateVulkanDataType(const BufferDataType& dataType)
	{
		switch (dataType)
		{
		case BufferDataType::Float:    return vk::Format::eR32Sfloat;
		case BufferDataType::Float2:   return vk::Format::eR32G32Sfloat;
		case BufferDataType::Float3:   return vk::Format::eR32G32B32Sfloat;
		case BufferDataType::Float4:   return vk::Format::eR32G32B32A32Sfloat;
		case BufferDataType::Int:      return vk::Format::eR32Sint;
		case BufferDataType::Int2:     return vk::Format::eR32G32Sint;
		case BufferDataType::Int3:     return vk::Format::eR32G32B32Sint;
		case BufferDataType::Int4:     return vk::Format::eR32G32B32A32Sint;
		default:
			PR_CORE_ASSERT(false, "Cannot determine Vulkan Format for BufferDatatype");
		}
	}

	VertexBuffer::VertexBuffer(const Prism::VertexBuffer::Layout& layout, uint32_t size, float* data)
	{
		m_Descriptor = GetVulkanDescriptor(layout);

		m_Buffer = MemoryManager::CreateBuffer(size, BufferType::VertexBuffer, BufferAccessPattern::GPU_static);
		MemoryManager::BufferData(size, data, *m_Buffer);
	}

	VertexBuffer::Descriptor VertexBuffer::GetVulkanDescriptor(const Prism::VertexBuffer::Layout& layout)
	{
		Descriptor result;

		constexpr uint32_t binding = 0;
		result.bindingDescription = vk::VertexInputBindingDescription(binding, layout.stride);

		uint32_t attribCount = layout.attributes.size();
		result.attributeDescriptions.resize(attribCount);

		for (uint32_t i = 0; i < attribCount; ++i)
		{
			result.attributeDescriptions[i] = vk::VertexInputAttributeDescription(i,
				binding,
				generateVulkanDataType(layout.attributes[i].type),
				layout.attributes[i].offset);
		}
		return result;
	}

	IndexBuffer::IndexBuffer(uint32_t size, uint32_t* data)
	{
		m_Buffer = MemoryManager::CreateBuffer(size, BufferType::IndexBuffer, BufferAccessPattern::GPU_static);
		MemoryManager::BufferData(size, data, *m_Buffer, 0);
	}
}

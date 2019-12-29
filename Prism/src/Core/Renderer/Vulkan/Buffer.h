#pragma once

#include "vma/vk_mem_alloc.h"
#include "vulkan/vulkan.hpp"

#include "Components/RenderComponent.h"
#include "../Buffer.h"

namespace Prism::Vulkan {

	enum class BufferAccessPattern
	{
		GPU_dynamic, // ressources most frequently used by GPU (e.g. render targets)
		GPU_static,  // static ressources frequently read by GPU (e.g. textures, vertex buffers)
		CPU_dynamic  // ressources frequently changed by CPU (e.g. push constants)
	};

	enum class BufferType
	{
		UniformBuffer = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		StorageBuffer = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		IndexBuffer = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VertexBuffer = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
	};

	struct Buffer
	{
		VkBuffer bufferHandle;
		VmaAllocation allocation;

		Buffer(VkBuffer bufferHandle, VmaAllocation allocation)
			: bufferHandle(bufferHandle), allocation(allocation) {}

		~Buffer();
	};

	class VertexBuffer : public Prism::VertexBuffer
	{
	public:
		VertexBuffer(const Prism::VertexBuffer::Layout& layout, uint32_t size, float* data);

		struct Descriptor
		{
			vk::VertexInputBindingDescription bindingDescription;
			std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
		};

		static Descriptor GetVulkanDescriptor(const Prism::VertexBuffer::Layout& genericDescriptor);

		const Descriptor& GetDescriptor() const { return m_Descriptor; };
		const Buffer& GetBuffer() const { return *m_Buffer; };

	private:
		Descriptor m_Descriptor;
		std::unique_ptr<Buffer> m_Buffer;
	};

	class IndexBuffer : public Prism::IndexBuffer
	{
	public:
		IndexBuffer(uint32_t size, uint32_t* data);

		const Buffer& GetBuffer() const { return *m_Buffer; };

	private:
		std::unique_ptr<Buffer> m_Buffer;
	};
}
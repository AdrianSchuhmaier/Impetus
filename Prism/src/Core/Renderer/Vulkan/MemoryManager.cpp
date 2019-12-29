#include "pch.h"
#include "MemoryManager.h"
#include "Context.h"

namespace Prism::Vulkan {

	VmaAllocator MemoryManager::m_Allocator;

	CommandPool MemoryManager::m_CommandPool;
	std::vector<uint32_t> MemoryManager::m_QueueFamilyIndices;

	void MemoryManager::Init()
	{
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = Context::GetPhysicalDevice();
		allocatorInfo.device = Context::GetDevice();

		vmaCreateAllocator(&allocatorInfo, &m_Allocator);

		if (Context::GetGraphicsQueue().familyIndex != Context::GetTransferQueue().familyIndex)
			m_QueueFamilyIndices = { Context::GetGraphicsQueue().familyIndex , Context::GetTransferQueue().familyIndex };
		else
			m_QueueFamilyIndices = { Context::GetTransferQueue().familyIndex };

		m_CommandPool = CommandPool(Context::GetTransferQueue().familyIndex);
	}

	void MemoryManager::CleanUp()
	{
		vmaDestroyAllocator(m_Allocator);
		m_CommandPool.~CommandPool();
	}

	std::unique_ptr<Buffer> MemoryManager::CreateBuffer(size_t size, BufferType type, BufferAccessPattern access)
	{
		VkBuffer buffer;
		VmaAllocation allocation;

		auto usage = static_cast<VkBufferUsageFlagBits>(type);
		switch (access)
		{
		case BufferAccessPattern::CPU_dynamic:
			PR_CORE_ASSERT(false, "Access pattern CPU_dynamic not supported yet.");

		case BufferAccessPattern::GPU_static: // TODO: decide for static content
		case BufferAccessPattern::GPU_dynamic:
			// create Buffer on Device
			VkBufferCreateInfo vbInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
			vbInfo.size = size;
			vbInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage;

			if (m_QueueFamilyIndices.size() == 1)
				vbInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			else
			{
				vbInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
				vbInfo.queueFamilyIndexCount = m_QueueFamilyIndices.size();
				vbInfo.pQueueFamilyIndices = m_QueueFamilyIndices.data();
			}

			VmaAllocationCreateInfo vbAllocCreateInfo = {};
			vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
			vbAllocCreateInfo.flags = 0;
			vmaCreateBuffer(m_Allocator, &vbInfo, &vbAllocCreateInfo, &buffer, &allocation, nullptr);

			return std::make_unique<Buffer>(buffer, allocation);
		}
		PR_CORE_ASSERT(false, "Buffer memory access undefined.");
	}

	void MemoryManager::BufferData(size_t size, void* data, const Buffer& buffer, uint32_t offset)
	{

		// create staging buffer on Host
		VkBufferCreateInfo vbInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		vbInfo.size = size;
		vbInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		if (true || m_QueueFamilyIndices.size() == 1)
			vbInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		else
		{
			vbInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
			vbInfo.queueFamilyIndexCount = m_QueueFamilyIndices.size();
			vbInfo.pQueueFamilyIndices = m_QueueFamilyIndices.data();
		}

		VmaAllocationCreateInfo vbAllocCreateInfo = {};
		vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		vbAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VkBuffer stagingVertexBuffer = VK_NULL_HANDLE;
		VmaAllocation stagingVertexBufferAlloc = VK_NULL_HANDLE;
		VmaAllocationInfo stagingVertexBufferAllocInfo = {};
		auto result = vmaCreateBuffer(m_Allocator, &vbInfo, &vbAllocCreateInfo, &stagingVertexBuffer, &stagingVertexBufferAlloc, &stagingVertexBufferAllocInfo);


		// copy data to staging buffer
		memcpy(stagingVertexBufferAllocInfo.pMappedData, data, size);

		// transfer data from staging buffer
		m_CommandPool.SubmitSingleUse(Context::GetTransferQueue().queue, [=, &buffer](vk::CommandBuffer cmd) {
			cmd.copyBuffer(stagingVertexBuffer, buffer.bufferHandle, vk::BufferCopy(0, 0, size));
			});


		// destroy staging buffer
		vmaDestroyBuffer(m_Allocator, stagingVertexBuffer, stagingVertexBufferAlloc);

	}
	void MemoryManager::FreeBuffer(Buffer* buffer)
	{
		vmaFreeMemory(m_Allocator, buffer->allocation);
		Context::GetDevice().destroyBuffer(buffer->bufferHandle);
	}
}
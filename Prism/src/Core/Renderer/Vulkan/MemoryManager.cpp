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
		m_CommandPool.~CommandPool();
		vmaDestroyAllocator(m_Allocator);
	}

	std::unique_ptr<Buffer> MemoryManager::CreateBuffer(size_t size, BufferType type, BufferAccessPattern access)
	{
		VkBuffer buffer;
		VmaAllocation allocation;
		VkBufferCreateInfo vbInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		VmaAllocationCreateInfo vbAllocCreateInfo = {};

		auto usage = static_cast<VkBufferUsageFlagBits>(type);
		switch (access)
		{
		case BufferAccessPattern::CPU_dynamic:
			vbInfo.size = size;
			vbInfo.usage = usage;

			if (true || m_QueueFamilyIndices.size() == 1)
				vbInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			else
			{
				vbInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
				vbInfo.queueFamilyIndexCount = m_QueueFamilyIndices.size();
				vbInfo.pQueueFamilyIndices = m_QueueFamilyIndices.data();
			}

			vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
			vbAllocCreateInfo.flags = 0;
			vmaCreateBuffer(m_Allocator, &vbInfo, &vbAllocCreateInfo, &buffer, &allocation, nullptr);

			return std::make_unique<Buffer>(buffer, allocation, access);

		case BufferAccessPattern::GPU_static: // TODO: decide for static content
		case BufferAccessPattern::GPU_dynamic:
			// create Buffer on Device
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

			vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
			vbAllocCreateInfo.flags = 0;
			vmaCreateBuffer(m_Allocator, &vbInfo, &vbAllocCreateInfo, &buffer, &allocation, nullptr);

			return std::make_unique<Buffer>(buffer, allocation, access);
		}
		return nullptr;
		PR_CORE_ASSERT(false, "Buffer memory access undefined.");
	}

	std::unique_ptr<Image> MemoryManager::CreateImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage)
	{
		VkImage image;
		VmaAllocation allocation;

		VkImageCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.imageType = VK_IMAGE_TYPE_2D;
		createInfo.extent.width = width;
		createInfo.extent.height = height;
		createInfo.extent.depth = 1;
		createInfo.mipLevels = 1;
		createInfo.arrayLayers = 1;
		createInfo.format = (VkFormat)format;
		createInfo.tiling = (VkImageTiling)tiling;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		createInfo.usage = (VkImageUsageFlags)usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		if (m_QueueFamilyIndices.size() == 1)
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		else
		{
			createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = m_QueueFamilyIndices.size();
			createInfo.pQueueFamilyIndices = m_QueueFamilyIndices.data();
		}

		VmaAllocationCreateInfo vbAllocCreateInfo = {};
		vbAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		vbAllocCreateInfo.flags = 0;

		auto res = vmaCreateImage(m_Allocator, &createInfo, &vbAllocCreateInfo, &image, &allocation, nullptr);

		return std::make_unique<Image>(image, allocation, width, height, (VkFormat)format);
	}

	void MemoryManager::BufferData(size_t size, void* data, const Buffer& buffer, uint32_t offset)
	{
		switch (buffer.access)
		{
		case BufferAccessPattern::CPU_dynamic:
			// no need for staging buffer, just map memory and memcpy to it
			void* mappedData;
			vmaMapMemory(m_Allocator, buffer.allocation, &mappedData);
			memcpy(mappedData, data, size);
			vmaUnmapMemory(m_Allocator, buffer.allocation);
			break;

		case BufferAccessPattern::GPU_static:
		case BufferAccessPattern::GPU_dynamic:
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
			vmaCreateBuffer(m_Allocator, &vbInfo, &vbAllocCreateInfo, &stagingVertexBuffer, &stagingVertexBufferAlloc, &stagingVertexBufferAllocInfo);


			// copy data to staging buffer
			memcpy(stagingVertexBufferAllocInfo.pMappedData, data, size);

			// transfer data from staging buffer
			m_CommandPool.SubmitSingleUse(Context::GetTransferQueue().queue, [=, &buffer](vk::CommandBuffer cmd) {
				cmd.copyBuffer(stagingVertexBuffer, buffer.bufferHandle, vk::BufferCopy(0, 0, size));
				});


			// destroy staging buffer
			vmaDestroyBuffer(m_Allocator, stagingVertexBuffer, stagingVertexBufferAlloc);
		}
	}

	void MemoryManager::BufferImage(size_t size, void* data, Image& image)
	{
		// create staging buffer on Host
		VkBufferCreateInfo stagingInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		stagingInfo.size = size;
		stagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		if (true || m_QueueFamilyIndices.size() == 1)
			stagingInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		else
		{
			stagingInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
			stagingInfo.queueFamilyIndexCount = m_QueueFamilyIndices.size();
			stagingInfo.pQueueFamilyIndices = m_QueueFamilyIndices.data();
		}

		VmaAllocationCreateInfo stagingAllocCreateInfo = {};
		stagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		stagingAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VmaAllocation stagingBufferAlloc = VK_NULL_HANDLE;
		VmaAllocationInfo stagingBufferAllocInfo = {};
		vmaCreateBuffer(m_Allocator, &stagingInfo, &stagingAllocCreateInfo, &stagingBuffer, &stagingBufferAlloc, &stagingBufferAllocInfo);

		// copy data to staging buffer
		memcpy(stagingBufferAllocInfo.pMappedData, data, size);


		m_CommandPool.SubmitSingleUse(Context::GetTransferQueue().queue, [=, &image](vk::CommandBuffer cmd) {
			image.TransitionLayout(cmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			VkBufferImageCopy region = {};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageOffset = { 0, 0, 0 };
			region.imageExtent = {
				image.width,
				image.height,
				1
			};
			vkCmdCopyBufferToImage(cmd, stagingBuffer, image.imageHandle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
			image.TransitionLayout(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			});


		// destroy staging buffer
		vmaDestroyBuffer(m_Allocator, stagingBuffer, stagingBufferAlloc);
	}

	void MemoryManager::FreeBuffer(Buffer* buffer)
	{
		vmaDestroyBuffer(m_Allocator, buffer->bufferHandle, buffer->allocation);
	}

	void MemoryManager::FreeImage(Image* image)
	{
		vmaDestroyImage(m_Allocator, image->imageHandle, image->allocation);
	}
}
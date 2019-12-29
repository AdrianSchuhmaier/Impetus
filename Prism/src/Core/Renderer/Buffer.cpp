#include "pch.h"
#include "Buffer.h"
#include "Vulkan/Buffer.h"

namespace Prism {

	std::unique_ptr<VertexBuffer> Prism::VertexBuffer::Create(const VertexBuffer::Layout& inputDescription, size_t size, float* data)
	{
		return std::make_unique<Vulkan::VertexBuffer>(inputDescription, size, data);
	}

	std::unique_ptr<IndexBuffer> IndexBuffer::Create(size_t size, uint32_t* data)
	{
		return std::make_unique<Vulkan::IndexBuffer>(size, data);
	}
}
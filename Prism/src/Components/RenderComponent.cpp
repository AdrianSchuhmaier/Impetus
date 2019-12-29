#include "pch.h"
#include "RenderComponent.h"

#include "Core/Renderer/Renderer.h"

namespace Prism {
	RenderComponent::RenderComponent(
		const std::string& shaderFile,
		const VertexBuffer::Layout& inputDescription,
		uint32_t vertexCount,
		float* vertices,
		uint32_t indexCount,
		uint32_t* indices
	)
		: material(std::make_shared<Material>(shaderFile, inputDescription))
		, mesh(std::make_shared<Mesh>())
	{
		mesh->vertexCount = indexCount;
		mesh->vertexBuffer = VertexBuffer::Create(
			inputDescription,
			(size_t)vertexCount * inputDescription.stride,
			vertices);
		mesh->indexBuffer = IndexBuffer::Create(
			indexCount * sizeof(uint32_t),
			indices);
	}
}
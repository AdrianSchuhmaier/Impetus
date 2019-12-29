#include "pch.h"
#include "RenderComponent.h"

#include "Core/Renderer/Renderer.h"

namespace Prism {
	RenderComponent::RenderComponent(
		const std::string& shaderFile,
		const VertexBuffer::Layout& inputDescription,
		uint64_t vertexCount,
		float* vertexData
	)
		: material(std::make_shared<Material>(shaderFile, inputDescription))
		, mesh(std::make_shared<Mesh>())
	{
		mesh->vertexCount = vertexCount;
		mesh->vertexBuffer = VertexBuffer::Create(
			inputDescription,
			vertexCount * inputDescription.stride,
			vertexData);
	}
}
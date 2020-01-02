#include "pch.h"
#include "RenderComponent.h"

#include "Core/Renderer/Renderer.h"

namespace Prism {
	RenderComponent::RenderComponent(
		const std::string& shaderFile,
		const VertexBuffer::Layout& inputDescription,
		uint32_t vertexCount,
		std::unique_ptr<VertexBuffer>&& vb,
		std::unique_ptr<IndexBuffer>&& ib
	)
		: material(std::make_shared<Material>(shaderFile, inputDescription))
		, mesh(std::make_shared<Mesh>())
	{
		mesh->vertexCount = vertexCount;
		mesh->vertexBuffer = std::move(vb);
		mesh->indexBuffer = std::move(ib);
	}
}
#pragma once

#include "Component.h"
#include "Core/Renderer/Mesh.h"
#include "Core/Renderer/Material.h"

namespace Prism {

	struct RenderComponent : public Component
	{
		std::shared_ptr<Mesh> mesh = nullptr;
		std::shared_ptr<Material> material = nullptr;

		RenderComponent() = default;
		RenderComponent(
			const std::string& shaderFile,
			const VertexBuffer::Layout& inputDescription,
			uint32_t vertexCount,
			std::unique_ptr<VertexBuffer>&& vb,
			std::unique_ptr<IndexBuffer>&& ib);
	};
}
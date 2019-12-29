#pragma once

#include "Component.h"
#include "Core/Renderer/Mesh.h"
#include "Core/Renderer/Material.h"

#include "Core/Renderer/RenderObject.h"

namespace Prism {

	struct RenderComponent : public Component
	{
		bool visible = true;
		std::shared_ptr<Mesh> mesh = nullptr;
		std::shared_ptr<Material> material = nullptr;

		RenderObjectHandle renderObject;

		RenderComponent() = default;
		RenderComponent(
			const std::string& shaderFile,
			const Prism::VertexBuffer::Layout& input,
			uint64_t vertexCount,
			float* vertexData);
	};
}
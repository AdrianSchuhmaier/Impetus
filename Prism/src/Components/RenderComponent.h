#pragma once

#include "Component.h"
#include "Core/Renderer/Mesh.h"
#include "Core/Renderer/Material.h"

#include "Core/Renderer/RenderObject.h"

namespace Prism {


	struct RenderAsset
	{
		std::string shaderFile;
		VertexInputDescription vertexInputDescription;
	};

	struct RenderComponent : public Component
	{
		bool visible = true;
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;

		RenderObjectHandle renderObject;

		RenderComponent(const RenderAsset& asset);
		~RenderComponent();
	};
}
#include "pch.h"
#include "RenderComponent.h"

#include "Core/Renderer/Renderer.h"

namespace Prism {
	RenderComponent::RenderComponent(const RenderAsset& asset)
		: material(std::make_shared<Material>(asset.shaderFile, asset.vertexInputDescription))
	{
		renderObject = Renderer::Register(*this);
	}

	RenderComponent::~RenderComponent()
	{
		Renderer::Unregister(renderObject);
	}
}
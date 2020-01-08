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
		RenderComponent(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material)
			: mesh(mesh), material(material) {}
	};
}
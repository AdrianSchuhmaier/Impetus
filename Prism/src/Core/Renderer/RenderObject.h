#pragma once

#include "Material.h"
#include "Mesh.h"

namespace Prism {

	using RenderObjectHandle = uint32_t;

	struct RenderObject
	{
		std::shared_ptr<Mesh> mesh;
		// Texture
		Material material;

		RenderObject(const std::shared_ptr<Mesh>& mesh, Material material)
			: mesh(mesh)
			, material(material){}
	};
}
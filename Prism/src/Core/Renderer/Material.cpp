#include "pch.h"
#include "Material.h"

#include "Vulkan/ShaderLibrary.h"

namespace Prism {

	Material::Material(
		const std::string& file,
		const VertexInputDescription& inputDescription
	) : shader(file)
	{
		ShaderLibrary::Load(shader, inputDescription);
	}

	Material::~Material()
	{
		// TODO: use reference-counting
		// ShaderLibrary::Remove(shader);
	}
}
#include "pch.h"
#include "Material.h"

#include "Vulkan/ShaderLibrary.h"

namespace Prism {

	Material::Material(
		const std::string& file,
		const VertexBuffer::Layout& inputDescription,
		const Properties& props
	) : shader(file), properties(props)
	{
		ShaderLibrary::Load(shader, inputDescription);
	}

	Material::~Material()
	{
		// TODO: use reference-counting
		// ShaderLibrary::Remove(shader);
	}
}
#include "pch.h"
#include "Shader.h"

#include "Vulkan/ShaderLibrary.h"

namespace Prism {
	Shader::Shader(const std::string& file, const VertexInputDescription& inputDescription) : std::string(file)
	{
		ShaderLibrary::Load(file, inputDescription);
	}

	Shader::~Shader()
	{
		ShaderLibrary::Remove(*this);
	}
}
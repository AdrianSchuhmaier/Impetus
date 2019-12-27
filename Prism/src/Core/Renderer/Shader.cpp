#include "pch.h"
#include "Shader.h"

#include "Vulkan/ShaderLibrary.h"

namespace Prism {
	Shader::Shader(const std::string& file) : std::string(file)
	{
		ShaderLibrary::Load(file);		
	}

	Shader::~Shader()
	{
		ShaderLibrary::Remove(*this);
	}
}
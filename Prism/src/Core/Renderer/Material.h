#pragma once

#include "BufferUtil.h"

namespace Prism {

	using ShaderHandle = std::string;

	struct Material
	{
		// shader filename is the handle
		ShaderHandle shader;

		// TODO: properties

		Material(const std::string& file, const VertexInputDescription& inputDescription);
		~Material();
	};
}
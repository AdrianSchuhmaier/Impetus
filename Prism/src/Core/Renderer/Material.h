#pragma once

#include "Buffer.h"

namespace Prism {

	using ShaderHandle = std::string;
	
	enum class ShaderType {
		Vertex, Fragment, Compute, Geometry, TesselationControl, TesselationEvaluation
	};

	using ShaderCode = std::unordered_map<ShaderType, std::string>;
	using ShaderBinary = std::unordered_map<ShaderType, std::vector<uint32_t>>;

	struct Material
	{
		// shader filename is the handle
		ShaderHandle shader;

		// TODO: properties

		Material() = default;
		Material(const std::string& file, const VertexBuffer::Layout& inputDescription);
		~Material();
	};
}
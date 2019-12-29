#pragma once

#include "BufferUtil.h"
#include <string>

namespace Prism {

	class Shader : public std::string
	{
		friend class ShaderLibrary;
	public:
		enum class Type {
			Vertex, Fragment, Compute, Geometry, TesselationControl, TesselationEvaluation
		};

		using Code = std::unordered_map<Shader::Type, std::string>;
		using SpirV = std::unordered_map<Shader::Type, std::vector<uint32_t>>;

		Shader() = default;
		Shader(const std::string& file, const VertexInputDescription& inputDescription);
		~Shader();
	};
}
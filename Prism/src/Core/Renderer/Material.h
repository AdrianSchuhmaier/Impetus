#pragma once

#include "Buffer.h"
#include "Texture.h"

namespace Prism {

	using ShaderHandle = std::string;

	enum class ShaderType {
		Vertex, Fragment, Compute, Geometry, TesselationControl, TesselationEvaluation
	};

	using ShaderCode = std::unordered_map<ShaderType, std::string>;
	using ShaderBinary = std::unordered_map<ShaderType, std::vector<uint32_t>>;

	struct Material
	{
		struct Properties {
			std::shared_ptr<Texture2D> texture = nullptr;

			Properties(const std::shared_ptr<Texture2D>& texture) : texture(texture) {}
		};

		// shader filename is the handle
		ShaderHandle shader;
		Properties properties;


		Material() = default;
		Material(const std::string& file, const VertexBuffer::Layout& inputDescription, const Properties& props = { nullptr });
		~Material();
	};
}
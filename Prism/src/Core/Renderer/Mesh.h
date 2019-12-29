#pragma once

#include "Buffer.h"

namespace Prism {

	struct Mesh
	{
		std::unique_ptr<VertexBuffer> vertexBuffer = nullptr;
		std::unique_ptr<IndexBuffer> indexBuffer = nullptr;
		uint32_t vertexCount = 0;
	};
}
#pragma once

#include "Buffer.h"

namespace Prism {

	struct Mesh
	{
		std::unique_ptr<VertexBuffer> vertexBuffer = nullptr;
		std::unique_ptr<IndexBuffer> indexBuffer = nullptr;
		uint32_t vertexCount = 0;

		Mesh(uint32_t vertexCount, std::unique_ptr<VertexBuffer>& vb, std::unique_ptr<IndexBuffer>& ib)
			: vertexBuffer(std::move(vb)), indexBuffer(std::move(ib)), vertexCount(vertexCount) {}
	};
}
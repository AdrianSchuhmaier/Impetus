#pragma once

#include "Core/Window/Window.h"

#include "RenderObject.h"

namespace Prism {
	class Renderer
	{
	public:
		static void Init(const Window* window);
		static void Shutdown();

		static void Prepare();
		static void Render();

		static void Register(const std::shared_ptr<RenderObject>&);

		static void Resize(uint32_t width, uint32_t height);

	private:
		static std::vector<std::weak_ptr<RenderObject>> renderObjects;
	};
}
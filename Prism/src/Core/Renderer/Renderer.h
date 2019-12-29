#pragma once

#include "Core/Window/Window.h"
#include "Core/Renderer/Buffer.h"

#include "Components/RenderComponent.h"

#include <set>

namespace Prism {
	class Renderer
	{
	public:
		static void Init(const Window* window);
		static void Shutdown();

		static void Prepare();
		static void Render();

		static void Resize(uint32_t width, uint32_t height);

		static RenderObjectHandle Register(const RenderComponent& component);
		static void Unregister(RenderObjectHandle component);

		// TODO: change datastructure
		static std::set<uint32_t> freeIndices;
		static std::vector<RenderObject> renderObjects;
	};
}
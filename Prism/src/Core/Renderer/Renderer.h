#pragma once

#include "Core/Window/Window.h"
#include "Core/Renderer/Buffer.h"

#include "Entity.h"

#include <unordered_set>

namespace Prism {

	class Renderer
	{
	public:
		static void Init(const Window* window);
		static void Shutdown();

		static void Prepare();
		static void Render();

		static void Resize(uint32_t width, uint32_t height);

		// Handle
		static void Register(EntityID entity);
		static void UseCamera(EntityID camera);
		//static void Unregister(RenderObjectHandle renderObject);
	};
}
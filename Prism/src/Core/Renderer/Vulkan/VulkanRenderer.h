#pragma once

#include "Core/Window/Window.h"

namespace Prism {
	class VulkanRenderer
	{
	public:
		static void Init(const Window* window);
		static void Shutdown();
	};
}
#pragma once

#include "Component.h"

namespace Prism {
	class RenderObject;

	struct RenderComponent : public Component
	{
		bool visible = true;
		std::shared_ptr<RenderObject> renderObject;
	};
}
#pragma once

#include "Component.h"
#include <glm/glm.hpp>

namespace Prism {

	struct CameraComponent : public Component
	{
		glm::mat4 projViewMatrix;
	};
}
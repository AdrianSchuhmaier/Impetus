#pragma once
#include "Component.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Prism {

	struct TransformComponent : public Component
	{
		glm::mat4 transform = glm::mat4(1.0f);
	};
}
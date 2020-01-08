#include "pch.h"
#include "Texture.h"
#include "Vulkan/Texture.h"


namespace Prism {
	std::shared_ptr<Texture2D> Texture2D::Create(const std::string& file)
	{
		return std::make_shared<Vulkan::Texture2D>(file);
	}
}

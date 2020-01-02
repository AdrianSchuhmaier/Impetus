#pragma once

#include "../Vulkan/RenderPass.h"
#include "../Vulkan/DescriptorPool.h"

namespace Prism::Vulkan {

	struct Defaults
	{
		static void Init();
		static void CleanUp();

		static RenderPass* GetDefaultRenderPass();
		static vk::DescriptorSetLayout GetDefaultUniformBufferDescriptor();

		static vk::PushConstantRange GetDefaultPushConstantRange();
	};

}
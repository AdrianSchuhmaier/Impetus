#pragma once

#include "../Vulkan/RenderPass.h"
#include "../Vulkan/DescriptorPool.h"

namespace Prism::Vulkan {

	struct Defaults
	{
		static void Init();
		static void CleanUp();

		static RenderPass* GetDefaultRenderPass();
		static vk::DescriptorSetLayout GetDefaultCameraDescriptor();
		static vk::DescriptorSetLayout GetDefaultUniformDescriptor();


		static vk::Sampler GetDefaultTextureSampler();

		static vk::PushConstantRange GetDefaultPushConstantRange();
	};

}
#include "pch.h"
#include "VulkanDefaults.h"

#include "../Vulkan/Context.h"

namespace Prism::Vulkan {

	RenderPass* s_DefaultRenderPass = nullptr;
	vk::DescriptorSetLayout s_DefaultCameraDescriptor = nullptr;
	vk::DescriptorSetLayout s_DefaultUniformDescriptor = nullptr;

	vk::Sampler s_TextureSampler = nullptr;

	void Defaults::Init()
	{

	}

	void Defaults::CleanUp()
	{
		if (s_DefaultRenderPass)
			delete s_DefaultRenderPass;

		if (s_DefaultCameraDescriptor)
			Context::GetDevice().destroyDescriptorSetLayout(s_DefaultCameraDescriptor);

		if (s_DefaultUniformDescriptor)
			Context::GetDevice().destroyDescriptorSetLayout(s_DefaultUniformDescriptor);

		if (s_TextureSampler)
			Context::GetDevice().destroySampler(s_TextureSampler);
	}

	RenderPass* Defaults::GetDefaultRenderPass() {
		if (!s_DefaultRenderPass)
		{
			RenderPass::RenderPassBP blueprint;
			blueprint.attachmentDescriptions.emplace_back(vk::AttachmentDescriptionFlags(),
				Context::GetSwapchain().format, vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eClear,		// color and depth
				vk::AttachmentStoreOp::eStore,
				vk::AttachmentLoadOp::eDontCare,	// stencil
				vk::AttachmentStoreOp::eDontCare,
				vk::ImageLayout::eUndefined,		// before
				vk::ImageLayout::ePresentSrcKHR);	// after

			blueprint.subpassDependencies.emplace_back(
				vk::SubpassDependency(VK_SUBPASS_EXTERNAL, 0,
					vk::PipelineStageFlagBits::eColorAttachmentOutput,
					vk::PipelineStageFlagBits::eColorAttachmentOutput,
					{}, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite));

			RenderPass::SubpassBP subpass;
			subpass.colorAttachments.emplace_back(0, vk::ImageLayout::eColorAttachmentOptimal);
			blueprint.subpassBlueprints.emplace_back(std::move(subpass));

			s_DefaultRenderPass = new RenderPass(blueprint);
		}

		return s_DefaultRenderPass;
	}

	vk::DescriptorSetLayout Defaults::GetDefaultCameraDescriptor()
	{
		if (!s_DefaultCameraDescriptor)
		{
			vk::DescriptorSetLayoutBinding cameraBinding(0,
				vk::DescriptorType::eUniformBuffer, 1,
				vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

			std::array<vk::DescriptorSetLayoutBinding, 1> bindings = { cameraBinding };

			vk::DescriptorSetLayoutCreateInfo createInfo(vk::DescriptorSetLayoutCreateFlags(),
				static_cast<uint32_t>(bindings.size()), bindings.data());

			s_DefaultCameraDescriptor = Context::GetDevice().createDescriptorSetLayout(createInfo);
		}

		return s_DefaultCameraDescriptor;
	}

	vk::DescriptorSetLayout Defaults::GetDefaultUniformDescriptor()
	{
		if (!s_DefaultUniformDescriptor)
		{
			vk::DescriptorSetLayoutBinding textureBinding(0,
				vk::DescriptorType::eCombinedImageSampler, 1,
				vk::ShaderStageFlagBits::eFragment);

			std::array<vk::DescriptorSetLayoutBinding, 1> bindings = { textureBinding };
			
			vk::DescriptorSetLayoutCreateInfo createInfo(vk::DescriptorSetLayoutCreateFlags(),
				static_cast<uint32_t>(bindings.size()), bindings.data());

			s_DefaultUniformDescriptor = Context::GetDevice().createDescriptorSetLayout(createInfo);
		}
		return s_DefaultUniformDescriptor;
	}

	vk::Sampler Defaults::GetDefaultTextureSampler()
	{
		if (!s_TextureSampler)
		{
			vk::SamplerCreateInfo samplerInfo = {};
			samplerInfo.magFilter = vk::Filter::eLinear;
			samplerInfo.minFilter = vk::Filter::eLinear;
			samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
			samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
			samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
			samplerInfo.anisotropyEnable = VK_TRUE;
			samplerInfo.maxAnisotropy = 16;
			samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
			samplerInfo.unnormalizedCoordinates = false;
			samplerInfo.compareEnable = false;
			samplerInfo.compareOp = vk::CompareOp::eAlways;
			samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;

			//TODO: anisotropy query

			s_TextureSampler = Context::GetDevice().createSampler(samplerInfo);
		}
		return s_TextureSampler;
	}

	vk::PushConstantRange Defaults::GetDefaultPushConstantRange()
	{
		return vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4));
	}
}
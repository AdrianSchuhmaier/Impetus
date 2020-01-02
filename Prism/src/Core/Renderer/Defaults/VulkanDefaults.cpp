#include "pch.h"
#include "VulkanDefaults.h"

#include "../Vulkan/Context.h"

namespace Prism::Vulkan {

	RenderPass* s_DefaultRenderPass = nullptr;
	vk::DescriptorSetLayout s_DefaultUniformBufferDescriptor = nullptr;

	void Defaults::Init()
	{

	}

	void Defaults::CleanUp()
	{
		if (s_DefaultRenderPass)
			delete s_DefaultRenderPass;

		if (s_DefaultUniformBufferDescriptor)
			Context::GetDevice().destroyDescriptorSetLayout(s_DefaultUniformBufferDescriptor);

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

	vk::DescriptorSetLayout Defaults::GetDefaultUniformBufferDescriptor()
	{
		if (!s_DefaultUniformBufferDescriptor)
		{
			vk::DescriptorSetLayoutBinding binding(0,
				vk::DescriptorType::eUniformBuffer, 1,
				vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
			vk::DescriptorSetLayoutCreateInfo createInfo(vk::DescriptorSetLayoutCreateFlags(), 1, &binding);

			s_DefaultUniformBufferDescriptor = Context::GetDevice().createDescriptorSetLayout(createInfo);
		}

		return s_DefaultUniformBufferDescriptor;
	}

	vk::PushConstantRange Defaults::GetDefaultPushConstantRange()
	{
		return vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4));
	}
}
#include "pch.h"
#include "RenderPass.h"
#include "Context.h"

namespace Prism::Vulkan {

	RenderPass* RenderPass::s_DefaultPass = nullptr;


	RenderPass::RenderPass(const RenderPassBP& blueprint)
	{
		std::vector<vk::SubpassDescription> subPasses(blueprint.subpassBlueprints.size());
		for (uint32_t i = 0; i < blueprint.subpassBlueprints.size(); ++i)
		{
			const auto& subPassBlueprint = blueprint.subpassBlueprints[i];
			subPasses[i].flags = {};
			subPasses[i].inputAttachmentCount = static_cast<uint32_t>(subPassBlueprint.inputAttachments.size());
			subPasses[i].colorAttachmentCount = static_cast<uint32_t>(subPassBlueprint.colorAttachments.size());
			subPasses[i].preserveAttachmentCount = static_cast<uint32_t>(subPassBlueprint.preserveAttachments.size());

			subPasses[i].pInputAttachments = subPassBlueprint.inputAttachments.data();
			subPasses[i].pColorAttachments = subPassBlueprint.colorAttachments.data();
			subPasses[i].pDepthStencilAttachment = subPassBlueprint.depthStencilAttachment;
			subPasses[i].pResolveAttachments = subPassBlueprint.resolveAttachments.data();
			subPasses[i].pPreserveAttachments = subPassBlueprint.preserveAttachments.data();

			subPasses[i].pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		}

		vk::RenderPassCreateInfo createInfo({},
			static_cast<uint32_t>(blueprint.attachmentDescriptions.size()), blueprint.attachmentDescriptions.data(),
			static_cast<uint32_t>(subPasses.size()), subPasses.data(),
			static_cast<uint32_t>(blueprint.subpassDependencies.size()), blueprint.subpassDependencies.data());

		m_RenderPass = Context::GetDevice().createRenderPassUnique(createInfo);
		SetClearValue(vk::ClearColorValue(std::array<float, 4> { {1.0f, 0.0f, 1.0f, 1.0f} }));
	}

	void RenderPass::Begin(
		vk::CommandBuffer cmd,
		vk::Framebuffer framebuffer,
		vk::SubpassContents nextSubpassType,
		std::optional<vk::Rect2D> renderArea) const
	{
		vk::RenderPassBeginInfo renderPassInfo(
			m_RenderPass.get(), framebuffer,
			renderArea.value_or(vk::Rect2D({ 0, 0 }, Context::GetSwapchain().extent)),
			1, &m_ClearValue);
		cmd.beginRenderPass(renderPassInfo, nextSubpassType);
	}

	void RenderPass::NextSubPass(vk::CommandBuffer cmd, vk::SubpassContents nextSubpassType) const
	{
		// TODO: check for validity
		cmd.nextSubpass(nextSubpassType);
	}

	void RenderPass::End(vk::CommandBuffer cmd) const
	{
		cmd.endRenderPass();
	}

	RenderPass& RenderPass::GetDefaultPass() {
		if (!s_DefaultPass)
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

			s_DefaultPass = new RenderPass(blueprint);
		}

		return *s_DefaultPass;
	}
}
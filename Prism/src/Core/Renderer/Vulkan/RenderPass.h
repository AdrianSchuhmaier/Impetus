#pragma once

#include "vulkan/vulkan.hpp"
#include <vector>

namespace Prism::Vulkan {

	class RenderPass
	{
	public:
		struct SubpassBP
		{
			std::vector<vk::AttachmentReference> inputAttachments = {};
			std::vector<vk::AttachmentReference> colorAttachments = {};
			std::vector<vk::AttachmentReference> resolveAttachments = {};
			vk::AttachmentReference* depthStencilAttachment = nullptr;
			std::vector<uint32_t> preserveAttachments = {};
		};

		struct RenderPassBP
		{
			std::vector<vk::AttachmentDescription> attachmentDescriptions = {};
			std::vector<SubpassBP> subpassBlueprints = {};
			std::vector<vk::SubpassDependency> subpassDependencies = {};
		};

		RenderPass() = default;
		RenderPass(const RenderPassBP& blueprint);
		~RenderPass() = default;

		void Begin(vk::CommandBuffer cmd, vk::Framebuffer framebuffer, vk::SubpassContents nextSubpassType,
			std::optional<vk::Rect2D> renderArea = std::nullopt) const;
		void NextSubPass(vk::CommandBuffer cmd, vk::SubpassContents nextSubpassType) const;
		void End(vk::CommandBuffer cmd) const; 

		void SetClearValue(vk::ClearValue clearValue) { m_ClearValue = clearValue; }
		vk::RenderPass GetHandle() const { return m_RenderPass.get(); }

		static RenderPass& GetDefaultPass();

		static void CleanUpStatic() {
			delete s_DefaultPass;
		}

	private:
		vk::UniqueRenderPass m_RenderPass;
		vk::ClearValue m_ClearValue;

		static RenderPass* s_DefaultPass;
	};
}
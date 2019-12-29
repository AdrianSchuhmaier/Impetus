#pragma once

#include <vulkan/vulkan.hpp>
#include "../Shader.h"

namespace Prism::Vulkan {

	struct VertexBufferDescriptor
	{
		vk::VertexInputBindingDescription bindingDescription;
		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
	};

	class Pipeline
	{
	public:
		Pipeline(
			const Shader::SpirV& code,
			const VertexBufferDescriptor& descriptor = {}, // TODO: remove
			const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts = {},
			const std::vector<vk::PushConstantRange>& pushConstants = {});

		~Pipeline();


		void SetVertexInput(
			const vk::VertexInputBindingDescription& bindingDescription,
			const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions,
			vk::PrimitiveTopology topology);

		void SetLayout(
			const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts,
			const std::vector<vk::PushConstantRange>& pushConstants);

		void SetShaders(const Shader::SpirV& spv);


		struct ColorBlend { vk::BlendFactor src, dst; vk::BlendOp op; };
		void SetColorBlend(const ColorBlend& color, const ColorBlend& alpha);

		void SetDynamicState(const std::vector<vk::DynamicState>& dynamicState);

		inline void SetRasterizationState(
			vk::CullModeFlags cullMode = vk::CullModeFlagBits::eNone,
			vk::FrontFace frontFace = vk::FrontFace::eCounterClockwise,
			vk::PolygonMode polygonMode = vk::PolygonMode::eFill);

		inline void SetMultisamplingState(
			vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1);

		// Builds the pipeline asynchronously
		void Create(vk::RenderPass renderPass, uint32_t subpassIndex = 0);

		void Bind(vk::CommandBuffer cmd);

	private:
		std::atomic<bool> m_Empty = true;
		std::atomic<uint8_t> m_Current = 0;
		vk::UniquePipeline m_Pipelines[2];

		uint32_t m_SubPassIndex;
		vk::RenderPass m_RenderPass;
		vk::UniquePipelineLayout m_Layout;
		vk::PipelineColorBlendAttachmentState m_ColorBlendAttachment;
		vk::PipelineColorBlendStateCreateInfo m_ColorBlendCreateInfo;
		vk::PipelineRasterizationStateCreateInfo m_RasterizationStateCreateInfo;
		vk::PipelineMultisampleStateCreateInfo m_MultisampleStateCreateInfo;
		std::vector<vk::DynamicState> m_DynamicStates;
		vk::PipelineDynamicStateCreateInfo m_DynamicStateCreateInfo;

		vk::VertexInputBindingDescription m_VertexInputBindingDescription;
		std::vector<vk::VertexInputAttributeDescription> m_VertexInputAttributeDescriptions;
		vk::PipelineVertexInputStateCreateInfo m_VertexInputCreateInfo;
		vk::PipelineInputAssemblyStateCreateInfo m_InputAssemblyCreateInfo;

		struct ShaderModule
		{
			vk::UniqueShaderModule module;
			vk::PipelineShaderStageCreateInfo info;
		};
		std::unordered_map<Shader::Type, ShaderModule> m_ShaderModules;
	};
}
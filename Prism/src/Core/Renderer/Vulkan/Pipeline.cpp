#include "pch.h"
#include "Pipeline.h"

#include "Context.h"

namespace Prism::Vulkan {

	Pipeline::Pipeline(
		const ShaderBinary& code,
		const VertexBuffer::Descriptor& descriptor,
		const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts,
		const std::vector<vk::PushConstantRange>& pushConstants)
	{
		SetLayout(descriptorSetLayouts, pushConstants);
		SetShaders(code);
		SetVertexInput(descriptor.bindingDescription, descriptor.attributeDescriptions, vk::PrimitiveTopology::eTriangleList);
		SetColorBlend(
			{ vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd },
			{ vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd });
		SetDynamicState({ vk::DynamicState::eViewport, vk::DynamicState::eScissor });
		SetRasterizationState();
		SetMultisamplingState();
	}

	void Pipeline::SetLayout(const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts,
		const std::vector<vk::PushConstantRange>& pushConstants)
	{
		m_Layout = Context::GetDevice().createPipelineLayoutUnique({ {},
			(uint32_t)descriptorSetLayouts.size(), descriptorSetLayouts.empty() ? nullptr : descriptorSetLayouts.data(),
			(uint32_t)pushConstants.size(), pushConstants.empty() ? nullptr : pushConstants.data() });
	}

	void Pipeline::SetVertexInput(const vk::VertexInputBindingDescription& bindingDescription,
		const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions,
		vk::PrimitiveTopology topology)
	{
		m_VertexInputBindingDescription = bindingDescription;
		m_VertexInputAttributeDescriptions = attributeDescriptions;

		m_VertexInputCreateInfo = vk::PipelineVertexInputStateCreateInfo({},
			1, &m_VertexInputBindingDescription,
			m_VertexInputAttributeDescriptions.size(),
			m_VertexInputAttributeDescriptions.data());

		m_InputAssemblyCreateInfo = vk::PipelineInputAssemblyStateCreateInfo({}, topology);
	}

	// defined below
	vk::ShaderStageFlagBits toVulkanStageFlag(const ShaderType& type);

	void Pipeline::SetShaders(const ShaderBinary& spv)
	{
		m_ShaderModules.clear();
		for (const auto& shader : spv)
		{
			m_ShaderModules[shader.first].module =
				Context::GetDevice().createShaderModuleUnique(
					vk::ShaderModuleCreateInfo({}, sizeof(uint32_t) * shader.second.size(), shader.second.data()));
			m_ShaderModules[shader.first].info = vk::PipelineShaderStageCreateInfo({},
				toVulkanStageFlag(shader.first), m_ShaderModules[shader.first].module.get(), "main");
		}
	}

	void Pipeline::SetColorBlend(const ColorBlend& color, const ColorBlend& alpha)
	{
		m_ColorBlendAttachment = { VK_FALSE,
			color.src, color.dst, color.op, // color
			alpha.src, alpha.dst, alpha.op, // alpha
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
			vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };
		m_ColorBlendCreateInfo = { {}, VK_FALSE, vk::LogicOp::eCopy, 1, &m_ColorBlendAttachment };
	}

	void Pipeline::SetDynamicState(const std::vector<vk::DynamicState>& dynamicState)
	{
		m_DynamicStates = dynamicState;
		m_DynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo({},
			m_DynamicStates.size(), m_DynamicStates.data());
	}

	inline void Pipeline::SetRasterizationState(
		vk::CullModeFlags cullMode, vk::FrontFace frontFace, vk::PolygonMode polygonMode)
	{
		m_RasterizationStateCreateInfo = { {},
			VK_FALSE, VK_FALSE, polygonMode, cullMode, frontFace,
			VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f };
	}

	inline void Pipeline::SetMultisamplingState(vk::SampleCountFlagBits sampleCount)
	{
		m_MultisampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo({},
			sampleCount, VK_FALSE, 1.0f, nullptr, VK_FALSE, VK_FALSE);
	}

	void Pipeline::Create(vk::RenderPass renderPass, const uint32_t subPassIndex)
	{
		m_RenderPass = renderPass;
		m_SubPassIndex = subPassIndex;

		//PR_CORE_ASSERT(!m_CreationLock, "Already creating a pipeline!");
		//m_CreationLock = new TaskLock(); // gets deleted in Bind
		//TaskSystem::Submit([&](uint32_t) {

			// Create Pipeline here
			PR_CORE_INFO("Pipeline creation...");

			// gather shaders
			std::vector<vk::PipelineShaderStageCreateInfo> stages;
			stages.reserve(5);

			// vertex + fragment shader must be there
			stages.emplace_back(m_ShaderModules.at(ShaderType::Vertex).info);
			stages.emplace_back(m_ShaderModules.at(ShaderType::Fragment).info);

			// add other shaders (if found)
			if (m_ShaderModules.find(ShaderType::Geometry) != m_ShaderModules.end())
				stages.emplace_back(m_ShaderModules.at(ShaderType::Geometry).info);
			if (m_ShaderModules.find(ShaderType::TesselationControl) != m_ShaderModules.end())
				stages.emplace_back(m_ShaderModules.at(ShaderType::TesselationControl).info);
			if (m_ShaderModules.find(ShaderType::TesselationEvaluation) != m_ShaderModules.end())
				stages.emplace_back(m_ShaderModules.at(ShaderType::TesselationEvaluation).info);

			vk::Viewport viewport(0.0f, 0.0f, Context::GetSwapchain().extent.width,
				Context::GetSwapchain().extent.height, 0.0f, 1.0f);
			vk::Rect2D scissor(vk::Offset2D(), Context::GetSwapchain().extent);

			vk::PipelineViewportStateCreateInfo viewportCreateInfo({},
				1, &viewport, 1, &scissor);

			vk::GraphicsPipelineCreateInfo pipelineCreateInfo({},
				stages.size(), stages.data(),
				&m_VertexInputCreateInfo,
				&m_InputAssemblyCreateInfo,
				nullptr, // tesselation
				&viewportCreateInfo,
				&m_RasterizationStateCreateInfo,
				&m_MultisampleStateCreateInfo,
				nullptr,					// depth & stencil
				&m_ColorBlendCreateInfo,
				&m_DynamicStateCreateInfo,
				m_Layout.get(),
				m_RenderPass,				// bind to renderpass
				m_SubPassIndex,				// + subpass index
				nullptr, -1);				// derivation

			uint8_t newIndex = (m_Current + 1) % 2;
			m_Pipelines[newIndex] = Context::GetDevice().createGraphicsPipelineUnique(nullptr, pipelineCreateInfo);

			// TODO: clean up shaderModules

			// Replace Pipeline:
			// should be atomic and Bind does only read once
			// -> no synchronization needed here
			m_Current = newIndex;
			m_Empty = false;
			PR_CORE_INFO("Pipeline created.");
			//}, m_CreationLock);
	}

	void Pipeline::Bind(vk::CommandBuffer cmd)
	{
		//if (m_Empty) && m_CreationLock)
		//{
		//	PR_CORE_WARN("Pipeline still in creation, waiting to bind...");
		//	m_CreationLock->wait();
		//}
		//
		//if (m_CreationLock && m_CreationLock->finished()) { delete m_CreationLock; m_CreationLock = nullptr; }

		PR_CORE_ASSERT(!m_Empty, "Cannot bind empty pipeline!");

		vk::Viewport viewport(0.0f, 0.0f, Context::GetSwapchain().extent.width,
			Context::GetSwapchain().extent.height, 0.0f, 1.0f);
		vk::Rect2D scissor(vk::Offset2D(), Context::GetSwapchain().extent);
		cmd.setViewport(0, 1, &viewport);
		cmd.setScissor(0, 1, &scissor);
		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipelines[m_Current].get());
	}

	vk::ShaderStageFlagBits toVulkanStageFlag(const ShaderType& type)
	{
		switch (type)
		{
		case ShaderType::Vertex:
			return vk::ShaderStageFlagBits::eVertex;
		case ShaderType::Fragment:
			return vk::ShaderStageFlagBits::eFragment;
		case ShaderType::Geometry:
			return vk::ShaderStageFlagBits::eGeometry;
		case ShaderType::Compute:
			return vk::ShaderStageFlagBits::eCompute;
		case ShaderType::TesselationControl:
			return vk::ShaderStageFlagBits::eTessellationControl;
		case ShaderType::TesselationEvaluation:
			return vk::ShaderStageFlagBits::eTessellationEvaluation;
		default:
			return vk::ShaderStageFlagBits::eAllGraphics;
		}
	}

	Pipeline::~Pipeline()
	{
		Context::GetDevice().waitIdle();
	}
}
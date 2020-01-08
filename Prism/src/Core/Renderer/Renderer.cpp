#include "pch.h"
#include "Renderer.h"
#include "Defaults/VulkanDefaults.h"

#include "Vulkan/Context.h"
#include "Vulkan/Frame.h"
#include "Vulkan/RenderPass.h"
#include "Vulkan/ShaderLibrary.h"
#include "Vulkan/MemoryManager.h"

#include "Components/CameraComponent.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
#include "Core/Application.h"

#include "Material.h"

namespace Prism {
	struct UniformPacket
	{
		// TODO: mesh uniform buffers + their descriptor sets here
		Vulkan::DescriptorSet textureDescriptor;
	};

	// ========= Tuples =====================================================
	struct MeshRenderData
	{
		std::shared_ptr<Mesh> mesh;
		Material material;
		glm::mat4 transform;

		UniformPacket* uniforms;

		MeshRenderData(const std::shared_ptr<Mesh>& mesh, const Material& material, const glm::mat4& transform, UniformPacket* uniforms)
			: mesh(mesh)
			, material(material)
			, transform(transform)
			, uniforms(uniforms) {}
	};
	// ======================================================================


	// ========= Frame System ===============================================
	constexpr uint8_t FRAME_COUNT = 1;
	constexpr uint8_t PACKET_COUNT = FRAME_COUNT + 1;

	struct FramePacket
	{
		std::unique_ptr<Vulkan::UniformBuffer> cameraDataBuffer = nullptr;
		Vulkan::DescriptorSet cameraDescriptor;

		std::vector<MeshRenderData> meshData;
	};

	uint8_t current = 0;
	Vulkan::Frame* frames; // must be constructed after Vulkan::Context
	Vulkan::Frame& currentFrame() { return frames[current]; }

	uint8_t currentPacketIndex = 0;
	FramePacket framePackets[PACKET_COUNT]; // can be constructed here
	FramePacket& currentFramePacket() { return framePackets[currentPacketIndex]; }

	void nextFrame()
	{
		current = (current + 1) % FRAME_COUNT;
		currentPacketIndex = (currentPacketIndex + 1) % PACKET_COUNT;
	}
	// ======================================================================

	Vulkan::RenderPass* renderPass;
	Vulkan::DescriptorPool* descriptorPool;

	// ========= RenderObject System ========================================
	CameraComponent* camera = nullptr;

	std::unordered_set<EntityID> entities{};
	std::unordered_map<EntityID, std::array<UniformPacket, PACKET_COUNT>> uniformPackets{};
	// ======================================================================

	// ========= Forward declarations =======================================
	void drawMesh(Mesh* mesh);
	// ======================================================================


	void Renderer::Init(const Window* window)
	{
		Vulkan::Context::Init(window);
		Vulkan::MemoryManager::Init();
		Vulkan::Defaults::Init();

		frames = new Vulkan::Frame[FRAME_COUNT];

		renderPass = Vulkan::Defaults::GetDefaultRenderPass();
		renderPass->SetClearValue(vk::ClearColorValue(std::array<float, 4>({ 0.2f, 0.2f, 0.2f, 1.0f })));

		std::vector<vk::DescriptorPoolSize> sizes{ {vk::DescriptorType::eUniformBufferDynamic, 1}, {vk::DescriptorType::eCombinedImageSampler, 1} };
		descriptorPool = new Vulkan::DescriptorPool(sizes, 2 * PACKET_COUNT);
	}

	void Renderer::Shutdown()
	{
		Vulkan::Context::GetDevice().waitIdle();

		ShaderLibrary::CleanUp();

		delete[] frames;
		for (auto& framePacket : framePackets)
			framePacket = {};

		uniformPackets.clear();
		entities.clear();

		delete descriptorPool;
		Vulkan::Defaults::CleanUp();
		Vulkan::MemoryManager::CleanUp();
		Vulkan::Context::CleanUp();
	}

	void Renderer::Prepare()
	{
		nextFrame();
		// currentFrame() might be still in-flight, but currentFramePacket can be filled
		currentFramePacket().meshData.clear();
		currentFramePacket().meshData.reserve(entities.size());

		if (!camera)
		{
			PR_CORE_ERROR("No camera registred to render.");
			return;
		}
		currentFramePacket().cameraDataBuffer->UpdateNow(&camera->projViewMatrix);

		for (const auto id : entities)
		{
			Entity* entity = Application::world->GetEntity(id);
			if (!entity)
			{
				// TODO: build remove list
				PR_CORE_ERROR("Cannot get Entity id {0}", id);
				continue;
			}

			RenderComponent* renderComponent = entity->Get<RenderComponent>();
			if (!renderComponent)
			{
				// TODO: build remove list
				PR_CORE_ERROR("Entity id {0} has no RenderComponent", id);
				continue;
			}

			TransformComponent* transformComponent = entity->Get<TransformComponent>();
			if (!transformComponent)
			{
				// TODO: build remove list
				PR_CORE_ERROR("Entity id {0} has no TransformComponent", id);
				continue;
			}

			UniformPacket* uniformPacket = &uniformPackets.find(id)->second[currentPacketIndex];
			// update mesh uniform buffers

			currentFramePacket().meshData.emplace_back(
				renderComponent->mesh,
				*renderComponent->material,
				transformComponent->transform,
				uniformPacket
			);
		}

		if (currentFramePacket().meshData.size() < entities.size())
			PR_CORE_WARN("Not all registred Entities are queued for rendering.");
	}

	void Renderer::Render()
	{
		currentFrame().Begin();

		// Render Meshes
		renderPass->Begin(
			currentFrame().GetCommandBuffer(),
			currentFrame().GetFramebuffer(),
			vk::SubpassContents::eInline);

		PR_CORE_ASSERT(currentFramePacket().meshData.size() > 0, "No meshes to render!");
		const auto& pipelineSample = ShaderLibrary::PipelineOf(currentFramePacket().meshData[0].material.shader);
		currentFrame().GetCommandBuffer().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineSample->GetLayout(), 0, { currentFramePacket().cameraDescriptor.GetHandle() }, {});

		for (const auto& meshData : currentFramePacket().meshData)
		{
			const auto& pipeline = ShaderLibrary::PipelineOf(meshData.material.shader);
			pipeline->Bind(currentFrame().GetCommandBuffer());

			currentFrame().GetCommandBuffer().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline->GetLayout(), 1, { meshData.uniforms->textureDescriptor.GetHandle() }, {});
			currentFrame().GetCommandBuffer().pushConstants(pipeline->GetLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(meshData.transform), &meshData.transform);
			drawMesh(meshData.mesh.get());
		}

		renderPass->End(currentFrame().GetCommandBuffer());
		currentFrame().End();
	}

	void drawMesh(Mesh* mesh)
	{
		const auto& vb = static_cast<Vulkan::VertexBuffer*>(mesh->vertexBuffer.get());
		const auto& ib = static_cast<Vulkan::IndexBuffer*>(mesh->indexBuffer.get());

		vk::Buffer vertexBuffers[] = { vb->GetBuffer().bufferHandle };
		vk::DeviceSize offsets[] = { 0 };
		currentFrame().GetCommandBuffer().bindVertexBuffers(0, 1, vertexBuffers, offsets);

		currentFrame().GetCommandBuffer().bindIndexBuffer(
			ib->GetBuffer().bufferHandle, 0, vk::IndexType::eUint32);

		currentFrame().GetCommandBuffer().drawIndexed(mesh->vertexCount, 1, 0, 0, 0);
	}


	void Renderer::Register(EntityID id)
	{
		entities.insert(id);

		Entity* entity = Application::world->GetEntity(id);
		if (entity)
		{
			RenderComponent* rc = entity->Get<RenderComponent>();
			if (rc && uniformPackets.find(id) == uniformPackets.end())
			{
				uniformPackets.insert({ id, std::array<UniformPacket, PACKET_COUNT>() });
				for (auto& packet : uniformPackets[id])
				{
					packet.textureDescriptor = descriptorPool->AllocateDescriptorSet(Vulkan::Defaults::GetDefaultUniformDescriptor());
					packet.textureDescriptor.Update(static_cast<Vulkan::Texture2D*>(rc->material->properties.texture.get()));
				}
				return;
			}
		}
		PR_CORE_ASSERT(false, "Cannot register this Entity as a RenderObject.");
	}

	void Renderer::UseCamera(EntityID id)
	{
		Entity* entity = Application::world->GetEntity(id);
		if (entity)
		{
			CameraComponent* cameraComponent = entity->Get<CameraComponent>();
			if (cameraComponent)
			{
				camera = cameraComponent;
				for (auto& frame : framePackets)
				{
					frame.cameraDataBuffer = std::make_unique<Vulkan::UniformBuffer>(sizeof(camera->projViewMatrix));
					frame.cameraDescriptor = descriptorPool->AllocateDescriptorSet(Vulkan::Defaults::GetDefaultCameraDescriptor());
					frame.cameraDescriptor.Update(frame.cameraDataBuffer.get());
				}
				return;
			}
		}
		PR_CORE_ASSERT(false, "Cannot register this Entity as a Camera.");
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		Vulkan::Context::Resize(width, height);

		auto projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 10.0f);
		projection[1][1] *= -1;
		auto view = glm::lookAt(glm::vec3(.5f, .5f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		camera->projViewMatrix = projection * view;
		// TODO: recreate Descriptorpool
	}
}
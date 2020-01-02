#include <Prism.h>


struct Game : public Prism::Application
{
	std::shared_ptr<Prism::Entity> object = nullptr;
	std::shared_ptr<Prism::Entity> camera = nullptr;

	Game() : Prism::Application({ "Game Project Impetus", 800, 900 })
	{
		LimitFPS(500);

		struct Vertex
		{
			glm::vec2 pos;
			glm::vec3 color;
		};

		auto inputDescription = Prism::VertexBuffer::Layout::Generate
			<Vertex>(
				&Vertex::pos,
				&Vertex::color);

		const std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
		};

		const std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0
		};


		auto projection = glm::perspective(glm::radians(45.0f), 2.0f, 0.1f, 10.0f);
		projection[1][1] *= -1;
		auto view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		camera = std::make_shared<Prism::Entity>();
		camera->AddComponent<Prism::CameraComponent>();
		camera->Get<Prism::CameraComponent>()->projViewMatrix = projection * view;


		auto&& vb = Prism::VertexBuffer::Create(inputDescription, sizeof(Vertex) * vertices.size(), (float*)vertices.data());
		auto&& ib = Prism::IndexBuffer::Create(sizeof(uint32_t) * indices.size(), (uint32_t*)indices.data());

		object = std::make_shared<Prism::Entity>();
		object->AddComponent<Prism::RenderComponent>("assets/shader/shader.glsl", inputDescription, (uint32_t)indices.size(), std::move(vb), std::move(ib));
		object->AddComponent<Prism::TransformComponent>();


		Prism::Renderer::UseCamera(camera->id);
		Prism::Renderer::Register(object->id);
	}

	void OnUpdate(float dt) override
	{
		PR_LOG_TRACE(dt);
		object->Get<Prism::TransformComponent>()->transform *= glm::rotate(glm::mat4(1.0f), dt * glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	void OnEvent(Prism::Event& e) override
	{
		PR_LOG_TRACE("Unhandled event in Game.");
	}
};

APPLICATION_ENTRY_POINT{
	return new Game();
}
#include <Prism.h>


struct Game : public Prism::Application
{
	std::shared_ptr<Prism::Entity> object = nullptr;
	std::shared_ptr<Prism::Entity> camera = nullptr;

	Game() : Prism::Application({ "Game Project Impetus", 800, 900 })
	{
		LimitFPS(240);

		struct Vertex
		{
			glm::vec3 pos;
			glm::vec2 color;
		};

		auto inputDescription = Prism::VertexBuffer::Layout::Generate
			<Vertex>(
				&Vertex::pos,
				&Vertex::color);

		const std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}}
		};

		const std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0
		};


		auto projection = glm::perspective(glm::radians(45.0f), 16.0f/9.0f, 0.1f, 10.0f);
		projection[1][1] *= -1;
		auto view = glm::lookAt(glm::vec3(0.f, .5f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		camera = std::make_shared<Prism::Entity>();
		camera->AddComponent<Prism::CameraComponent>();
		camera->Get<Prism::CameraComponent>()->projViewMatrix = projection * view;

		auto&& vb = Prism::VertexBuffer::Create(inputDescription, sizeof(Vertex) * vertices.size(), (float*)vertices.data());
		auto&& ib = Prism::IndexBuffer::Create(sizeof(uint32_t) * indices.size(), (uint32_t*)indices.data());
		auto matprops = Prism::Material::Properties(Prism::Texture2D::Create("assets/textures/scarlett.jpg"));

		object = std::make_shared<Prism::Entity>();
		object->AddComponent<Prism::RenderComponent>(
			std::make_shared<Prism::Mesh>((uint32_t)indices.size(), std::move(vb), std::move(ib)),
			std::make_shared<Prism::Material>("assets/shader/shader.glsl", inputDescription, matprops));
		object->AddComponent<Prism::TransformComponent>();


		Prism::Renderer::UseCamera(camera->id);
		Prism::Renderer::Register(object->id);
	}

	void OnUpdate(float dt) override
	{
		object->Get<Prism::TransformComponent>()->transform = glm::rotate(glm::mat4(1.0f), (float)GetTime() * glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	void OnEvent(Prism::Event& e) override
	{
		PR_LOG_TRACE("Unhandled event in Game.");
	}
};

APPLICATION_ENTRY_POINT{
	return new Game();
}
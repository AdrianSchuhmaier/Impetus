#include <Prism.h>


struct Game : public Prism::Application
{
	std::shared_ptr<Prism::RenderComponent> renderObject = nullptr;

	Game() : Prism::Application({ "Game Project Impetus", 1800, 900 })
	{
		LimitFPS(240);

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

		// construct demo object
		renderObject = std::make_shared<Prism::RenderComponent>(
			"assets/shader/shader.glsl",
			inputDescription,
			vertices.size(),
			(float*)vertices.data(),
			indices.size(),
			(uint32_t*)indices.data());

		Prism::Renderer::Register(*renderObject);
	}

	void OnEvent(Prism::Event& e) override {
		PR_LOG_TRACE("Unhandled event in Game.");
	}
};

APPLICATION_ENTRY_POINT{
	return new Game();
}
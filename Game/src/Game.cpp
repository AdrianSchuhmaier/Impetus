#include <Prism.h>


struct Game : public Prism::Application
{
	std::shared_ptr<Prism::RenderComponent> renderObject = nullptr;

	Game() : Prism::Application({ "Game Project Impetus", 1800, 900 })
	{
		LimitFPS(240);

		struct Test
		{
			glm::vec2 pos;
			glm::vec3 color;
		};

		auto inputDescription = Prism::VertexInputDescription::Generate
			<Test>(
				&Test::pos,
				&Test::color);

		Prism::RenderAsset asset{ "assets/shader/shader.glsl", inputDescription };

		// construct demo object
		renderObject = std::make_shared<Prism::RenderComponent>(asset);
	}

	void OnEvent(Prism::Event& e) override {
		PR_LOG_TRACE("Unhandled event in Game.");
	}
};

APPLICATION_ENTRY_POINT{
	return new Game();
}
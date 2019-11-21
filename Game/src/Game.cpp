#include <Prism.h>

class Game : public Prism::Application
{
	void OnEvent(Prism::Event& e) override {
		PR_LOG_TRACE("Unhandled event in Game.");
	}
};

APPLICATION_ENTRY_POINT{
	return new Game();
}
#include <Prism.h>

class Game : public Prism::Application
{
};

APPLICATION_ENTRY_POINT{
	return new Game();
}
#pragma once

namespace Prism {
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		//void EventCallback(Event&);

	private:
		bool m_Running = true;
	};

	// defined in the client application
	Application* CreateApplication();
}
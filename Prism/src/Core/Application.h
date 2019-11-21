#pragma once

#include "Core/Window/Window.h"

namespace Prism {
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void EventCallback(Event&);

		virtual void OnEvent(Event&) = 0;

	private:
		std::unique_ptr<Window> m_Window = nullptr;
		bool m_Running = true;
	};

	// defined in the client application
	Application* CreateApplication();
}
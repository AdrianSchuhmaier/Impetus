#include "pch.h"
#include "Application.h"
#include "Renderer/Renderer.h"

namespace Prism {

	World* Application::world = nullptr;

	Application::Application(const Window::Properties& props)
	{
		m_Window = std::make_unique<Window>(props);
		m_Window->SetEventCallback(PR_BIND_EVENT_FN(Application::EventCallback));
		// start the Entity Component System
		world = new World();


		Renderer::Init(m_Window.get());
		PR_CORE_TRACE("Application created.");

		m_LastFrameTime = GetTime();
	}

	void Application::Run()
	{
		while (m_Running)
		{
			auto dt = GetDeltaTime();
			m_Window->OnUpdate();

			if (dt >= m_MinFrameDuration && !m_Minimized)
			{
				/*clientApp->*/OnUpdate(dt);

				Renderer::Prepare();
				Renderer::Render();
				StepFrame();
			}
		}
	}

	void Application::EventCallback(Event& event)
	{
		event.Handle<WindowCloseEvent>([&](WindowCloseEvent& e)
			{
				PR_CORE_TRACE("WindowClose handled by Application");
				m_Running = false;
				return true;
			});

		event.Handle<WindowResizeEvent>([&](WindowResizeEvent& e)
			{
				if (e.GetWidth() <= 0 || e.GetHeight() <= 0)
				{
					m_Minimized = true;
					return true;
				}
				Renderer::Resize(e.GetWidth(), e.GetHeight());
				m_Minimized = false;
				return true;
			});
		// Unhandled events: invoke OnEvent on the client application
		if (!event.handled) /*clientApp->*/OnEvent(event);
	}

	Application::~Application()
	{
		delete world;
		Renderer::Shutdown();
		PR_CORE_TRACE("Application destroyed.");
	}
}

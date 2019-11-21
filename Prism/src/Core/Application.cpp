#include "pch.h"
#include "Application.h"
#include "Renderer/Renderer.h"

namespace Prism {
	Application::Application()
	{
		m_Window = Window::Create({ "Project Impetus", 1280, 720, true });
		m_Window->SetEventCallback(PR_BIND_EVENT_FN(Application::EventCallback));

		Renderer::Init(m_Window.get());
		PR_CORE_TRACE("Application created.");
	}

	Application::~Application()
	{
		Renderer::Shutdown();
		PR_CORE_TRACE("Application destroyed.");
	}

	void Application::Run()
	{
		while (m_Running)
		{
			m_Window->OnUpdate();
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
				Renderer::Resize(e.GetWidth(), e.GetHeight());
				return true;
			});

		// Unhandled events: invoke OnEvent on the client application
		if (!event.handled) OnEvent(event);
	}
}

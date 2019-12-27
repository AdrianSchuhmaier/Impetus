#include "pch.h"
#include "Application.h"
#include "Renderer/Renderer.h"

namespace Prism {

	Application::Application()
	{
		m_Window = Window::Create({ "Fullscreen", 1920, 1080, true, false });
		m_Window->SetEventCallback(PR_BIND_EVENT_FN(Application::EventCallback));

		Renderer::Init(m_Window.get());
		PR_CORE_TRACE("Application created.");

		m_LastFrameTime = GetTime();
		LimitFPS(120);
	}

	void Application::Run()
	{
		while (m_Running)
		{
			auto dt = GetDeltaTime();

			m_Window->OnUpdate();

			if (dt >= m_MinFrameDuration && !m_Minimized)
			{
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
		if (!event.handled) OnEvent(event);
	}

	Application::~Application()
	{
		Renderer::Shutdown();
		PR_CORE_TRACE("Application destroyed.");
	}
}

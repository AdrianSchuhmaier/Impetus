#pragma once

#include "Core/Window/Window.h"

namespace Prism {
	class Application
	{
	public:
		Application(const Window::Properties& props =
			{ "Prism Engine", 1024, 720, false, true });
		virtual ~Application();

		void Run();
		void EventCallback(Event&);

		virtual void OnEvent(Event&) = 0;

	protected:
		double GetTime() { return m_Window->GetTime(); }
		float GetDeltaTime() { return (float)(GetTime() - m_LastFrameTime); }
		void StepFrame() { m_LastFrameTime = GetTime(); }
		void LimitFPS(float fps) { m_MinFrameDuration = 1.0f / fps; }
		void UnlimitFPS() { m_MinFrameDuration = 0.0f; }

	private:
		std::unique_ptr<Window> m_Window = nullptr;
		bool m_Running = true;
		bool m_Minimized = false;

		double m_LastFrameTime;
		float m_MinFrameDuration = 0;
	};

	// defined in the client application
	Application* CreateApplication();
}
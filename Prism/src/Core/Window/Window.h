#pragma once

#include "Core/Event.h"
#include "Log/Log.h"

namespace Prism {
	class Window
	{
	public:
		struct Properties
		{
			std::string title = "Prism";
			uint32_t width = 1280, height = 720;
			bool fullscreen = false;
			bool resizable = true;
		};

		using EventCallbackFn = std::function<void(Event&)>;

	private:

		// Data struct for GLFW callbacks
		struct WindowData
		{
			Properties& properties;
			EventCallbackFn callback = [](Event&) {
				PR_CORE_WARN("No window callback set");
			};
		};

		void SetGLFWCallbacks();


	public:
		Window(const Properties&);
		~Window();

		void OnUpdate();
		double GetTime() const;

		void SetEventCallback(const EventCallbackFn&);

		void* GetWindowHandle() const { return m_WindowHandle; }
		

	private:
		Properties m_Properties;
		WindowData m_WindowData;

		void* m_WindowHandle = nullptr; // GLFWwindow*

		static bool s_Initialized;
	};
}
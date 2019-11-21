#pragma once

#include "Core/Event.h"

namespace Prism {
	class Window
	{
	public:
		struct Properties
		{
			std::string title = "Prism";
			uint32_t width = 1280, height = 720;
			bool resizable = false;
		};

		using EventCallbackFn = std::function<void(Event&)>;

	private:

		// Data struct for GLFW callbacks
		struct WindowData
		{
			Properties& properties;
			EventCallbackFn callback;
		};

		void SetGLFWCallbacks();


	public:
		static std::unique_ptr<Window> Create(Properties = Properties());

		Window(Properties&);
		~Window();

		void OnUpdate();
		double GetTime() const;

		void SetEventCallback(const EventCallbackFn&);

		void* GetWindowHandle() const { return m_WindowHandle; }
		

	private:
		Properties m_Properties;
		WindowData m_WindowData;
		EventCallbackFn m_Callback;

		void* m_WindowHandle = nullptr; // GLFWwindow*

		static bool s_Initialized;
	};
}
#include "pch.h"
#include "Window.h"

#include "GLFW/glfw3.h"

namespace Prism {

	bool Window::s_Initialized = false;

	// required as a function pointer for GLFW
	static void GLFWErrorCallback(int error, const char* description)
	{
		PR_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window::Window(const Properties& props)
		: m_Properties{ props }
		, m_WindowData{ m_Properties }
	{
		if (!s_Initialized)
		{
			s_Initialized = glfwInit();
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		PR_CORE_ASSERT(s_Initialized, "Window initialization failed!");

		GLFWmonitor* monitor = nullptr;

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, m_Properties.resizable);

		if (m_Properties.fullscreen)
		{
			monitor = glfwGetPrimaryMonitor();
			auto mode = glfwGetVideoMode(monitor);
			m_Properties.width = mode->width;
			m_Properties.height = mode->height;
		}

		m_WindowHandle = glfwCreateWindow(m_Properties.width, m_Properties.height,
			m_Properties.title.c_str(), monitor, nullptr);

		SetGLFWCallbacks();
	}

	Window::~Window()
	{
		if (m_WindowHandle)
			glfwDestroyWindow((GLFWwindow*)m_WindowHandle);
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
	}

	double Window::GetTime() const
	{
		return glfwGetTime();
	}

	void Window::SetEventCallback(const EventCallbackFn& callback)
	{
		m_WindowData.callback = callback;
	}

	void Window::SetGLFWCallbacks()
	{
		GLFWwindow* window = (GLFWwindow*)m_WindowHandle;

		// set pointer to WindowData to use for GLFW callback
		glfwSetWindowUserPointer(window, &m_WindowData);

		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			data.properties.width = width;
			data.properties.height = height;

			WindowResizeEvent event(width, height);
			data.callback(event);
			});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.callback(event);
			});
	}
}
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

	std::unique_ptr<Window> Window::Create(Properties props)
	{
		if (!s_Initialized)
		{
			s_Initialized = glfwInit();
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		PR_CORE_ASSERT(s_Initialized, "Window initialization failed!");
		
		GLFWmonitor* monitor = nullptr;

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, props.resizable);
		
		if (props.fullscreen)
		{			
			monitor = glfwGetPrimaryMonitor();
			auto mode = glfwGetVideoMode(monitor);
			props.width = mode->width;
			props.height = mode->height;
		}

		auto window = std::make_unique<Window>(props);
		window->m_WindowHandle = glfwCreateWindow(props.width, props.height,
			props.title.c_str(), monitor, nullptr);

		window->SetGLFWCallbacks();

		return window;
	}

	Window::Window(Properties& props)
		: m_Properties{ props }
		, m_WindowData{ props }
	{
		m_WindowData.callback = [](Event& e) {
			PR_CORE_WARN("Cannot invoke callback for window - no callback set!");
		};
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
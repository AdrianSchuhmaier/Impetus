#include "pch.h"
#include "Application.h"

namespace Prism {
	Application::Application()
	{
	}

	Application::~Application()
	{
		std::cout << "destructed" << std::endl;
	}

	void Application::Run()
	{
		while (m_Running)
		{
			std::cout << "running..." << std::endl;
		}
	}
}

#pragma once

#include "Application.h"

#define APPLICATION_ENTRY_POINT Prism::Application* Prism::CreateApplication()
extern Prism::Application* Prism::CreateApplication();

int main(int argc, char** argv)
{
	Prism::Application* app = Prism::CreateApplication();
	app->Run();
	delete app;

	return 0;
}
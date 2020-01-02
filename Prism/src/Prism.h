#pragma once

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include "Core/Application.h"
#include "Core/Renderer/Renderer.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
#include "Entity.h"
#include "Log/Log.h"

// Entry point - must be defined (return Application*) instead of a main method
#define APPLICATION_ENTRY_POINT Prism::Application* Prism::CreateApplication()
extern Prism::Application* Prism::CreateApplication();

// Main method is defined here, games using Prism only define the entry point!
int main(int argc, char** argv)
{
	Prism::Log::Init();
	Prism::Application* app = Prism::CreateApplication();

	PR_CORE_HEAD("Running application");
	app->Run();


	delete app;

	return 0;
}
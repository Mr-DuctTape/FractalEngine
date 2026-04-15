#include "FractalEngineCore.h"
#include "../SceneManagement/FractalScene.h"
#include "../Rendering/RenderingSystem.h"
#include <iostream>

SDL_Window* FractalEngineCore::window = nullptr;
unsigned int FractalEngineCore::width = 0;
unsigned int FractalEngineCore::height = 0;
float FractalEngineCore::deltaTime = 0;
Uint64 previous;

void FractalEngineCore::Initialize()
{
	if (!SceneManager::GetCurrentScene())
		SceneManager::CreateDefault();
}

void FractalEngineCore::CreateWindow(const char* title, const int w, const int h, SDL_WindowFlags theme)
{
	previous = 0;
	width = w;
	height = h;
	window = SDL_CreateWindow(title, w, h, theme);
	if (!window)
	{
		std::cout << "Fractal Error: Failed to create window\n";
	}
	Rendering::Init(window);
	FractalEngineCore::Initialize();
}

void FractalEngineCore::DeltaTime() //Background stuff like deltatime etc
{
	if (previous == 0)
		previous = SDL_GetPerformanceCounter();

	int x, y;

	SDL_GetWindowSizeInPixels(window, &x, &y);
	FractalEngineCore::width = x;
	FractalEngineCore::height = y;

	Uint64 now = SDL_GetPerformanceCounter();
	FractalEngineCore::deltaTime = (float)(now - previous) / SDL_GetPerformanceFrequency();
	previous = now;
}
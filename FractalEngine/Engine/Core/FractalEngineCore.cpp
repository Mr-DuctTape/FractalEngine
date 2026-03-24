#include "FractalEngineCore.h"
#include "../SceneManagement/FractalScene.h"

#include <iostream>

SDL_Renderer* FractalEngineCore::renderer = nullptr;
SDL_Window* FractalEngineCore::window = nullptr;
unsigned int FractalEngineCore::width = 0;
unsigned int FractalEngineCore::height = 0;
float FractalEngineCore::deltaTime = 0;
Uint64 previous;

void FractalEngineCore::Initialize()
{
	if (!SceneManager::getCurrentScene())
		SceneManager::createDefault();
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
	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer)
	{
		std::cout << "Fractal Error: Failed to create renderer\n";
	}
	FractalEngineCore::Initialize();
}

void FractalEngineCore::Run() //Background stuff like deltatime etc
{
	if (previous == 0)
		previous = SDL_GetPerformanceCounter();

	Uint64 now = SDL_GetPerformanceCounter();
	FractalEngineCore::deltaTime = (float)(now - previous) / SDL_GetPerformanceFrequency();
	previous = now;
}

void FractalEngineCore::Quit()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}
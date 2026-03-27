#pragma once
#include <SDL3/SDL.h>

namespace FractalEngineCore
{
	void CreateWindow(const char* title, const int width, const int height, const SDL_WindowFlags theme);
	void Initialize();
	void DeltaTime();

	extern unsigned int width;
	extern unsigned int height;
	extern float deltaTime;

	extern SDL_Window* window;
}
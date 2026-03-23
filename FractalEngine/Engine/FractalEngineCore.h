#pragma once
#include <SDL3/SDL.h>

namespace FractalEngineCore
{
	extern unsigned int width;
	extern unsigned int height;
	extern float deltaTime;

	extern SDL_Window* window;
	extern SDL_Renderer* renderer;

	void Initialize(const char* title, const int width, const int height, const SDL_WindowFlags theme);
	void Quit();
	void Run();
}
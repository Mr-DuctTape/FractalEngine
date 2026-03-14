#include "FractalEngine.h"
#include "./Physics/PhysicsFunctions.h"
#include <iostream>

SDL_Renderer* FractalEngine::renderer = nullptr;
SDL_Window* FractalEngine::window = nullptr;

void FractalEngine::Initialize(const char* title, const int width, const int height, SDL_WindowFlags theme)
{
	window = SDL_CreateWindow(title, width, height, theme);
	if (!window)
	{
		std::cout << "Fractal Error: Failed to create window\n";
	}
	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer)
	{
		std::cout << "Fractal Error: Failed to create renderer\n";
	}
}

void FractalEngine::Quit()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}
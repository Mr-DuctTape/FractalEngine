#include "FractalEngine.h"
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

void FractalEngine::Update()
{
	for (auto& obj : objects)
	{
		auto c = obj.get();
		if (c->physics2D)
		{

		}
	}
}
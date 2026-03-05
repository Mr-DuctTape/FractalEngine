#pragma once
#include <SDL3/SDL.h>
#include <vector>

namespace Rendering
{
	extern std::vector<int> indices;
	extern std::vector<SDL_Vertex> vertices;

	void drawScreen();
	void clearScreen();
	void clearScreen(SDL_Color color);
}
#pragma once
#include <SDL3/SDL.h>
#include <vector>

namespace Rendering
{
	void drawScreen();
	void clearScreen();
	void clearScreen(SDL_Color color);
}
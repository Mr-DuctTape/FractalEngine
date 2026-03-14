#pragma once
#include "Input/InputSystem.h"
#include "Vectors/Vector2D.h"
#include "EntitySystem/Entities.h"
#include "Rendering/RenderingSystem.h"

#include <SDL3/SDL.h>

namespace FractalEngine
{
	extern SDL_Window* window;
	extern SDL_Renderer* renderer;
	void Initialize(const char* title, const int width, const int height, const SDL_WindowFlags theme);
	void Quit();
}
#pragma once
#include <SDL3/SDL.h>

namespace Input
{
	extern SDL_Event event;
	extern bool previousKeys[SDL_SCANCODE_COUNT];
	extern bool currentKeys[SDL_SCANCODE_COUNT];

	const bool getButton(SDL_Scancode key);
	const bool getButtonDown(SDL_Scancode key);
	const bool getButtonUp(SDL_Scancode key);
	void Process();
}
#pragma once
#include <SDL3/SDL.h>

namespace Input
{
	extern SDL_Event event;
	extern bool previousKeys[SDL_SCANCODE_COUNT];
	extern bool currentKeys[SDL_SCANCODE_COUNT];

	bool getButton(SDL_Scancode key);
	bool getButtonDown(SDL_Scancode key);
	bool getButtonUp(SDL_Scancode key);
	void process();
}
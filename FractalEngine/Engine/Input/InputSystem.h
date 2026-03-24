#pragma once
#include <SDL3/SDL.h>

class Input
{
private:
	static bool previousKeys[SDL_SCANCODE_COUNT];
	static bool currentKeys[SDL_SCANCODE_COUNT];
public:
	const static bool getButton(SDL_Scancode key);
	const static bool getButtonDown(SDL_Scancode key);
	const static bool getButtonUp(SDL_Scancode key);
	static void Process();
};
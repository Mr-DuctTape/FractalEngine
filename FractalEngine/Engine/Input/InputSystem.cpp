#include "InputSystem.h"

SDL_Event Input::event;

bool Input::currentKeys[SDL_SCANCODE_COUNT] = { false };
bool Input::previousKeys[SDL_SCANCODE_COUNT] = { false };

void Input::process()
{
	memcpy(previousKeys, currentKeys, sizeof(currentKeys));

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_EVENT_KEY_DOWN)
			currentKeys[e.key.scancode] = true;
		if (e.type == SDL_EVENT_KEY_UP)
			currentKeys[e.key.scancode] = false;
	}
}

bool Input::getButtonDown(SDL_Scancode key)
{
	return currentKeys[key] && !previousKeys[key];
}

bool Input::getButtonUp(SDL_Scancode key)
{
	return !currentKeys[key] && previousKeys[key];
}

bool Input::getButton(SDL_Scancode key)
{
	return currentKeys[key] && previousKeys[key];
}
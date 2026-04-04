#include "InputSystem.h"
#include "../FractalEngine.h"

bool Input::currentKeys[SDL_SCANCODE_COUNT] = { false };
bool Input::previousKeys[SDL_SCANCODE_COUNT] = { false };
bool Input::currentMouse[MOUSE_BUTTON_COUNT] = { false };
bool Input::previousMouse[MOUSE_BUTTON_COUNT] = { false };

void Input::Process()
{
	memcpy(previousKeys, currentKeys, sizeof(currentKeys));
	memcpy(previousMouse, currentMouse, sizeof(currentMouse));

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_EVENT_WINDOW_RESIZED)
	    {
			int x, y;
			SDL_GetWindowSize(FractalEngineCore::window, &x, &y);
			FractalEngineCore::width = x;
			FractalEngineCore::width = y;
			std::cout << "Window resized to " << FractalEngineCore::width << "x" << FractalEngineCore::height << std::endl;
		}

		if (e.type == SDL_EVENT_KEY_DOWN)
			currentKeys[e.key.scancode] = true;
		if (e.type == SDL_EVENT_KEY_UP)
			currentKeys[e.key.scancode] = false;

		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
			currentMouse[e.button.button] = true;
		if (e.type == SDL_EVENT_MOUSE_BUTTON_UP)
			currentMouse[e.button.button] = false;
	}
}

const Input::MousePosition Input::GetMousePosition() //Gets mouse position relative to the window
{
	float x, y;
	SDL_GetMouseState(&x, &y);
	return Input::MousePosition{x , y};
}

const bool Input::GetMouseButtonDown(Mouse button)
{
	return currentMouse[button] && !previousMouse[button];
}

const bool Input::GetMouseButtonUp(Mouse button)
{
	return !currentMouse[button] && previousMouse[button];
}

const bool Input::GetMouseButton(Mouse button)
{
	return currentMouse[button] && previousMouse[button];
}

const bool Input::GetButtonDown(SDL_Scancode key)
{
	return currentKeys[key] && !previousKeys[key];
}

const bool Input::GetButtonUp(SDL_Scancode key)
{
	return !currentKeys[key] && previousKeys[key];
}

const bool Input::GetButton(SDL_Scancode key)
{
	return currentKeys[key] && previousKeys[key];
}
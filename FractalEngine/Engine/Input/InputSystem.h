#pragma once
#include <SDL3/SDL.h>

enum Mouse
{
	LEFT_BUTTON = 1,
	MIDDLE_BUTTON,
	RIGHT_BUTTON,
	EXTRA_BUTTON1,
	EXTRA_BUTTON2,
	MOUSE_BUTTON_COUNT = 5
};

class Input
{
private:
	static bool previousKeys[SDL_SCANCODE_COUNT];
	static bool currentKeys[SDL_SCANCODE_COUNT];
	static bool previousMouse[MOUSE_BUTTON_COUNT];
	static bool currentMouse[MOUSE_BUTTON_COUNT];

public:
	struct MousePosition
	{
		float x, y;
	};
	static const bool getButton(SDL_Scancode key);
	static const bool getButtonDown(SDL_Scancode key);
	static const bool getButtonUp(SDL_Scancode key);
	static const bool getMouseButtonDown(Mouse button);
	static const bool getMouseButton(Mouse button);
	static const bool getMouseButtonUp(Mouse button);
	static const MousePosition getMousePosition();
	static void Process();
};
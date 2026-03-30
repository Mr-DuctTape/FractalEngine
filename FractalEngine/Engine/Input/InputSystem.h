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
	static const bool GetButton(SDL_Scancode key);
	static const bool GetButtonDown(SDL_Scancode key);
	static const bool GetButtonUp(SDL_Scancode key);
	static const bool GetMouseButtonDown(Mouse button);
	static const bool GetMouseButton(Mouse button);
	static const bool GetMouseButtonUp(Mouse button);
	static const MousePosition GetMousePosition();
	static void Process();
};
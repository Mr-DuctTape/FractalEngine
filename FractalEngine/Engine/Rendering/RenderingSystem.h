#pragma once
#include <SDL3/SDL.h>
#include <vector>

class Scene;

class Rendering
{
public:
	static void clearScreen();
	static void clearScreen(SDL_Color color);
	static void drawScreen(Scene* scene);
};
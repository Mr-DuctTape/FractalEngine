#include "FractalEngine.h"
#include <iostream>
#include <SDL3/SDL.h>
#include <vector>

int main()
{
	FractalEngine::Initialize("Hello", 1280, 720, SDL_WINDOW_BORDERLESS);
	while (true)
	{
		Input::Process();
		std::cout << Input::getButton(SDL_SCANCODE_K) << "\n";
	}
	return 0;
}
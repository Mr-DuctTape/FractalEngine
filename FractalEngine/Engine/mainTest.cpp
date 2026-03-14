#include "FractalEngine.h"
#include <iostream>
#include <SDL3/SDL.h>
#include <vector>

int main()
{
	FractalEngine::Initialize("Hello", 1280, 720, SDL_WINDOW_RESIZABLE);

	GameObject& obj = CreateObject<GameObject>();
	GameObject& obj2 = CreateObject<GameObject>();

	obj.addComponent<Components::Physics2D>();
	obj2.addComponent<Components::Physics2D>();

	obj.transform.position.x = 600;
	obj.transform.position.y = 300;
	obj2.transform.position.x = 300;
	obj2.transform.position.y = 300;

	while (true)
	{
		Input::Process();
		Physics::Run();
		Rendering::clearScreen();
		Rendering::drawScreen();
		obj2.transform.position.x += 1;
	}

	FractalEngine::Quit();

	return 0;
}
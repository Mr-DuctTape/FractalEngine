#include "FractalEngine.h"
#include <iostream>
#include <SDL3/SDL.h>
#include <vector>
#include "Physics/PhysicsFunctions.h"

int main()
{
	FractalEngine::Initialize("Hello", 1280, 720, SDL_WINDOW_RESIZABLE);

	GameObject& obj = CreateObject<GameObject>();

	obj.physics2D = addComponent<Physics2D>(nullptr);

	obj.transform.position.x = 600;
	obj.transform.position.y = 300;

	while (true)
	{
		Input::Process();
		Physics::Run();
		Rendering::clearScreen();
		Rendering::drawScreen();
	}

	FractalEngine::Quit();

	return 0;
}
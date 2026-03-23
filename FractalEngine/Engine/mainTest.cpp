#include "FractalEngine.h"
#include <iostream>

int main()
{
	FractalEngineCore::Initialize("Hello", 1920, 1080, SDL_WINDOW_RESIZABLE);

	GameObject& obj = CreateObject<GameObject>();
	GameObject& obj2 = CreateObject<GameObject>();
	GameObject& obj3 = CreateObject<GameObject>();
	GameObject& obj4 = CreateObject<GameObject>();

	obj.addComponent<Components::Physics2D>(Components::Physics2D());
	obj2.addComponent<Components::Physics2D>(Components::Physics2D());
	obj4.addComponent<Components::Physics2D>(Components::Physics2D());

	obj3.transform.position.x = 600;
	obj3.transform.position.y = 800;

	obj.transform.position.x = 600;
	obj.transform.position.y = 0;

	obj2.transform.position.x = 600;
	obj2.transform.position.y = 200;

	obj4.transform.position.x = 600;
	obj4.transform.position.y = -200;
	 
	obj2.getComponent<Components::Physics2D>()->mass = 20;

	bool bing = true;

	while (bing)
	{
		FractalEngineCore::Run();
		Input::Process();
		std::cout << FractalEngineCore::deltaTime << "\n";
		if (Input::getButtonDown(SDL_SCANCODE_2))
		{
			Components::Physics2D* phys = obj4.getComponent<Components::Physics2D>();
			phys->addForce({0.0, -50.0});
		}
		Physics::Run();
		Rendering::clearScreen();
		Rendering::drawScreen();
	}

	FractalEngineCore::Quit();
	return 0;
}
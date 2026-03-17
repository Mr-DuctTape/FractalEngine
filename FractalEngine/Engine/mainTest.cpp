#include "FractalEngine.h"

int main()
{
	FractalEngine::Initialize("Hello", 1280, 720, SDL_WINDOW_RESIZABLE);

	GameObject& obj = CreateObject<GameObject>();
	GameObject& obj2 = CreateObject<GameObject>();
	GameObject& obj3 = CreateObject<GameObject>();

	obj.addComponent<Components::Physics2D>(Components::Physics2D(10,20,30,40));
	obj2.addComponent<Components::Physics2D>(Components::Physics2D(10, 20, 30, 40));

	obj3.transform.position.x = 600;
	obj3.transform.position.y = 550;

	obj.transform.position.x = 600;
	obj.transform.position.y = 0;

	obj2.transform.position.x = 600;
	obj2.transform.position.y = 200;
	 
	std::cout << obj.getComponent<Components::Physics2D>()->mass << "\n";
	bool bing = true;
	while (bing)
	{
		Input::Process();
		Physics::Run();
		if (Input::getButton(SDL_SCANCODE_SPACE))
		{
			obj.transform.position.y -= 10;
			std::cout << "True\n";
		}
		if (Input::getButton(SDL_SCANCODE_0))
		{
			bing = false;
		}

		Rendering::clearScreen();
		Rendering::drawScreen();
	}

	FractalEngine::Quit();
	return 0;
}
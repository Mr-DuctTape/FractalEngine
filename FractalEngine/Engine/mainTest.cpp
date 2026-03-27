#include "FractalEngine.h"
#include <iostream>

int main()
{
	FractalEngine::Initialize();

	GameObject& obj = CreateObject<GameObject>();
	GameObject& obj2 = CreateObject<GameObject>();
	GameObject& obj3 = CreateObject<GameObject>();
	GameObject& obj4 = CreateObject<GameObject>();

	SceneManager::createScene("Test2");
	SceneManager::loadScene("Test2");

	GameObject& test = CreateObject<GameObject>();
	GameObject& test2 = CreateObject<GameObject>();

	test.transform.position = { 600, 500 };
	test2.transform.position = { 600, 50 };

	test2.addComponent<Components::Physics2D>();
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
	SceneManager::loadScene("Default");
	while (FractalEngine::running)
	{
		FractalEngine::start();
		if (Input::getButtonDown(SDL_SCANCODE_0))
		{
			obj4.getComponent<Components::Physics2D>()->addForce({ 0.0, -50.0 });
		}
		if (Input::getButton(SDL_SCANCODE_SPACE))
			obj4.transform.rotation++;
		if (Input::getButtonDown(SDL_SCANCODE_1))
			FractalEngine::stop();
		if (Input::getButtonDown(SDL_SCANCODE_2))
			SceneManager::loadScene("Test2");
		if (Input::getButtonDown(SDL_SCANCODE_3))
			SceneManager::loadScene("Default");
	}

	return 0;
}
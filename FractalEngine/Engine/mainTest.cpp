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

	while (FractalEngine::running)
	{
		Scene* current = SceneManager::getCurrentScene();
		std::cout << "\033[2J\033[1;1H";
		std::cout << "Current scene: " << current->name << "\n";
		FractalEngine::start();
		if (Input::getButtonDown(SDL_SCANCODE_0))
			SceneManager::loadScene("Default");
		if (Input::getButtonDown(SDL_SCANCODE_3))
			SceneManager::loadScene("Test2");
		if (Input::getButtonDown(SDL_SCANCODE_4))
			FractalEngine::stop();
	}

	return 0;
}
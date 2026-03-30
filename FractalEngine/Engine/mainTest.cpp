#include "FractalEngine.h"
#include <iostream>

int main()
{
	FractalEngine::Initialize();

	GameObject& obj = CreateObject<GameObject>();
	GameObject& obj2 = CreateObject<GameObject>();
	GameObject& obj3 = CreateObject<GameObject>();
	GameObject& obj4 = CreateObject<GameObject>();

	SceneManager::CreateScene("Test2");
	SceneManager::LoadScene("Test2");

	GameObject& test = CreateObject<GameObject>();
	GameObject& test2 = CreateObject<GameObject>();

	test.transform.position = { 600, 500 };
	test2.transform.position = { 600, 50 };
	test2.AddComponent<Components::Physics2D>();
	obj.AddComponent<Components::Physics2D>(Components::Physics2D());
	obj2.AddComponent<Components::Physics2D>(Components::Physics2D());
	obj4.AddComponent<Components::Physics2D>(Components::Physics2D());

	obj3.transform.position.x = 600;
	obj3.transform.position.y = 800;
	obj.transform.position.x = 600;
	obj.transform.position.y = 0;
	obj2.transform.position.x = 600;
	obj2.transform.position.y = 200;
	obj4.transform.position.x = 600;
	obj4.transform.position.y = -200;

	obj.AddComponent<Components::Animator>();
	Components::Animator* animator = obj.GetComponent<Components::Animator>();
	obj2.AddComponent<Components::Animator>();

	AssetManager::CreateTexture("Bob", "C:\\Users\\Ebisu\\source\\repos\\FractalEngine\\FractalEngine\\Textures\\bob.bmp");
	animator->CreateAnimation("BobAnimation", 5, 0.1f, AssetManager::GetTexture("Bob"));
	animator->SetAnimation("BobAnimation");

	obj2.GetComponent<Components::Animator>()->SetAnimation("BobAnimation");
	obj2.GetComponent<Components::Animator>()->SetSpeed(0.2f);
	obj2.GetComponent<Components::Animator>()->Play();

	animator->Play();

	SceneManager::LoadScene("Default");
	while (FractalEngine::running)
	{
		FractalEngine::Run();
		if (Input::GetButtonDown(SDL_SCANCODE_0))
		{
			obj4.GetComponent<Components::Physics2D>()->addForce({ 0.0, -50.0 });
		}
		if (Input::GetMouseButtonDown(LEFT_BUTTON))
			std::cout << "Pressed mouse button!\n";

		if (Input::GetButton(SDL_SCANCODE_SPACE))
			obj4.transform.rotation++;
		if (Input::GetButtonDown(SDL_SCANCODE_1))
			FractalEngine::Stop();
		if (Input::GetButtonDown(SDL_SCANCODE_2))
			SceneManager::LoadScene("Test2");
		if (Input::GetButtonDown(SDL_SCANCODE_3))
			SceneManager::LoadScene("Default");
	}

	return 0;
}
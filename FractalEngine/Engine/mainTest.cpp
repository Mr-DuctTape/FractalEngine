#include "FractalEngine.h"

#include <crtdbg.h>
#include <windows.h>
#include <psapi.h>
#include <iostream>

int main()
{
	// Initialize the engine

	_CrtMemState s1, s2, s3;
	_CrtMemCheckpoint(&s1);

	FractalEngine::Initialize();

	// Create objects on default scene
	GameObject& obj = CreateObject<GameObject>();
	GameObject& obj2 = CreateObject<GameObject>();

	auto tileMap = AssetManager::CreateTileMap("Tilemap1", "C:\\Users\\Ebisu\\source\\repos\\FractalEngine\\FractalEngine\\Textures\\Test.tilemap");
	tileMap->SetTileScale(5.0f, 5.0f);
	tileMap->SetTileCollidable(1, true);
	tileMap->position = { 0,0 };
	
	// Create scene and load it
	SceneManager::CreateScene("Test2");
	SceneManager::LoadScene("Test2");

	// Create objects on "Test2" scene
	GameObject& test = CreateObject<GameObject>();
	GameObject& test2 = CreateObject<GameObject>();

	// Set positions and sizes
	test.transform.position = { 600, 500 };
	test2.transform.position = { 600, 50 };

	obj.transform.position = { 600, -500 };
	obj2.transform.position = { 2000, -200 };

	// Add components
	obj.AddComponent<Components::Physics2D>();
	obj2.AddComponent<Components::Physics2D>();
	test2.AddComponent<Components::Physics2D>();

	obj.AddComponent<Components::Animator>();
	test.AddComponent<Components::Animator>();
	test2.AddComponent<Components::Animator>();

	// Apply animation to other objects
	Components::Animator* a = test2.GetComponent<Components::Animator>();
	a->SetAnimation("BobAnimation");
	a->SetSpeed(0.3f);

	Components::Animator* b = test.GetComponent<Components::Animator>();
	b->SetAnimation("BobAnimation");
	b->SetSpeed(0.5f);

	bool flipped = false;

	AssetManager::CreateTexture("Tilemap", "C:\\Users\\Ebisu\\source\\repos\\FractalEngine\\FractalEngine\\Textures\\TileMap.bmp");

	// Load default scene

	Components::Physics2D* phys = obj.GetComponent<Components::Physics2D>();

	SceneManager::LoadScene("Default");
	while (FractalEngine::running)
	{
		camera.follow(obj);
		FractalEngine::Run();
		if (Input::GetButtonDown(SDL_SCANCODE_1))
			FractalEngine::Stop();
	/*	if (Input::GetButtonDown(SDL_SCANCODE_2))
			SceneManager::LoadScene("Test2");*/
		if (Input::GetButtonDown(SDL_SCANCODE_3))
			SceneManager::LoadScene("Default");

		if (Input::GetButtonDown(SDL_SCANCODE_SPACE))
			tileMap->debugMode = TileMap::TileDebugMode::FULL;
		if (Input::GetButtonDown(SDL_SCANCODE_V))
			tileMap->debugMode = TileMap::TileDebugMode::NEARBY;
		if (Input::GetButtonDown(SDL_SCANCODE_B))
			tileMap->debugMode = TileMap::TileDebugMode::NONE;

		if (!phys) continue;
		if (Input::GetButton(SDL_SCANCODE_W))
		{
			phys->AddForce({ 0.0f, -1.0f });  // negative Y = up
		}
		if (Input::GetButton(SDL_SCANCODE_S))
		{
			phys->AddForce({ 0.0f, 1.0f });   // positive Y = down
		}
		if (Input::GetButton(SDL_SCANCODE_A))
		{
			phys->AddForce({ -1.0f, 0.0f });  // negative X = left
		}
		if (Input::GetButton(SDL_SCANCODE_D))
		{
			phys->AddForce({ 1.0f, 0.0f });   // positive X = right
		}
	}

	return 0;
}
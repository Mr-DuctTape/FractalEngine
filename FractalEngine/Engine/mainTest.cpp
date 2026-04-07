#include "FractalEngine.h"
#include <iostream>

int main()
{
	// Initialize the engine
	FractalEngine::Initialize();

	// Create objects on default scene
	GameObject& obj = CreateObject<GameObject>();
	GameObject& obj2 = CreateObject<GameObject>();
	TileMap& tilemap = CreateObject<TileMap>();

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
	obj2.transform.position = { 500, -200 };

	// Add components
	obj.AddComponent<Components::Physics2D>();
	obj2.AddComponent<Components::Physics2D>();
	test2.AddComponent<Components::Physics2D>();

	obj.AddComponent<Components::Animator>();
	test.AddComponent<Components::Animator>();
	test2.AddComponent<Components::Animator>();

	// Setup animation
	//Components::Animator* animator = obj.GetComponent<Components::Animator>();
	//AssetManager::CreateTexture("Bob", "C:\\Users\\Ebisu\\source\\repos\\FractalEngine\\FractalEngine\\Textures\\bob.bmp");
	//animator->CreateAnimation("BobAnimation", 5, 0.1f, AssetManager::GetTexture("Bob"));
	//animator->SetAnimation("BobAnimation");

	// Apply animation to other objects
	Components::Animator* a = test2.GetComponent<Components::Animator>();
	a->SetAnimation("BobAnimation");
	a->SetSpeed(0.3f);

	Components::Animator* b = test.GetComponent<Components::Animator>();
	b->SetAnimation("BobAnimation");
	b->SetSpeed(0.5f);

	bool flipped = false;

	AssetManager::CreateTexture("Tilemap", "C:\\Users\\Ebisu\\source\\repos\\FractalEngine\\FractalEngine\\Textures\\TileMap.bmp");

	const char* path = "C:\\Users\\Ebisu\\source\\repos\\FractalEngine\\FractalEngine\\Textures\\Tilemap.txt";
	tilemap.LoadTileMap(path);
	tilemap.SetTileSet(AssetManager::GetTexture("Tilemap"), 2);
	tilemap.SetTileScale(5.0f, 5.0f);
	tilemap.SetTilePixels(32, 32);
	tilemap.PrintTileMap();
	tilemap.SetTileCollidable(1, true);
	std::cout << tilemap.GetTileCollisionBox(4, 0) << "\n";
	// Load default scene
	SceneManager::LoadScene("Default");
	tilemap.position = { 0,0 };
	while (FractalEngine::running)
	{
		camera.follow(obj);
		FractalEngine::Run();
		if (Input::GetButtonDown(SDL_SCANCODE_1))
			FractalEngine::Stop();
		if (Input::GetButtonDown(SDL_SCANCODE_2))
			SceneManager::LoadScene("Test2");
		if (Input::GetButtonDown(SDL_SCANCODE_3))
			SceneManager::LoadScene("Default");

		if (Input::GetButtonDown(SDL_SCANCODE_SPACE) && !flipped)
		{
			flipped = true;
			//obj.GetComponent<Components::Sprite>()->flippedX = true;
		}
		else if (Input::GetButtonDown(SDL_SCANCODE_SPACE) && flipped)
		{
			flipped = false;
			//obj.GetComponent<Components::Sprite>()->flippedX = false;
		}

		Components::Physics2D* phys = obj.GetComponent<Components::Physics2D>();
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
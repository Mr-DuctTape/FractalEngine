#include "FractalEngine.h"

#include <crtdbg.h>
#include <windows.h>
#include <psapi.h>
#include <iostream>



int main()
{
	// Initialize the engine
	FractalEngine::Initialize();

	// Create objects on default scene
	GameObject& obj = CreateObject<GameObject>();
	GameObject& obj2 = CreateObject<GameObject>();

	auto tileMap = AssetManager::CreateTileMap("Tilemap1", "C:\\Users\\Ebisu\\source\\repos\\FractalEngine\\FractalEngine\\Textures\\Test.tilemap");
	tileMap->SetTileScale(5.0f, 5.0f);
	tileMap->SetTileSolid(1, true);
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
	obj2.transform.position = { 400, -200 };

	// Add components
	obj.AddComponent<Components::Physics2D>();
	obj2.AddComponent<Components::Physics2D>();
	test2.AddComponent<Components::Physics2D>();

	obj.AddComponent<Components::Animator>();
	test.AddComponent<Components::Animator>();
	test2.AddComponent<Components::Animator>();

	AssetManager::CreateTexture("Bob", "C:\\Users\\Ebisu\\source\\repos\\FractalEngine\\FractalEngine\\Textures\\bob.bmp");

	// Apply animation to other objects
	Components::Animator* a = test2.GetComponent<Components::Animator>();
	a->CreateAnimation("BobAnimation", 5, 0.1f, AssetManager::GetTexture("Bob"));
	a->SetAnimation("BobAnimation");
	a->SetSpeed(0.3f);
	a->Play();

	Components::Animator* b = test.GetComponent<Components::Animator>();
	b->SetAnimation("BobAnimation");
	b->SetSpeed(0.5f);
	b->Play();

	// Load default scene
	SceneManager::LoadScene("Default");
	Components::Physics2D* phys = obj.GetComponent<Components::Physics2D>();


	obj2.AddComponent<Components::Collider2D>();
	obj.AddComponent<Components::Collider2D>();

	obj2.GetComponent<Components::Collider2D>()->AddCollisionLayer(Layer::ENEMY);

	auto f = obj.GetComponent<Components::Collider2D>();
	f->AddCollisionLayer(Layer::PLAYER);

	std::cout << "Collision masks:\n";
	std::cout << f->GetCollisionMask() << "\n"; 

	tileMap->SetTileLayer(1, Layer::GROUND);
	std::cout << "Collision Mask for Tile 1: "
		<< tileMap->GetTileCollisionMask(1) << '\n';
	tileMap->AddTileCollisionLayer(1, Layer::ENEMY);
	std::cout << "Collision Mask for Tile 1: "
		<< tileMap->GetTileCollisionMask(1) << '\n';
	tileMap->AddTileCollisionLayer(1, Layer::PLAYER);
	std::cout << "Collision Mask for Tile 1: "
		<< tileMap->GetTileCollisionMask(1) << '\n';

	std::cout << "Tile ID at (2, 2): "
		<< tileMap->GetTileID(2, 2) << '\n';

	std::cout << "Layer for Tile 1: "
		<< tileMap->GetTileLayer(1) << '\n';

	while (FractalEngine::running)
	{
		camera.follow(obj);
		//Rendering::Debug::DrawCollisionBox(f->collisionBox, { 199, 122, 122, 255 }, true);
		FractalEngine::Run();
		if (Input::GetButtonDown(SDL_SCANCODE_1))
			FractalEngine::Stop();
		if (Input::GetButtonDown(SDL_SCANCODE_2))
			SceneManager::LoadScene("Test2");
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
			phys->AddForce({ -1.25f, 0.0f });  // negative X = left
		}
		if (Input::GetButton(SDL_SCANCODE_D))
		{
			phys->AddForce({ 1.25f, 0.0f });   // positive X = right
		}
	}

	return 0;
}
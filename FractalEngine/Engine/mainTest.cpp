#include "FractalEngine.h"
#include "EntitySystem/PrefabManager.h"
#include <crtdbg.h>
#include <windows.h>
#include <psapi.h>
#include <iostream>


int main()
{
	// Initialize the engine
	FractalEngine::Initialize();

	// Create objects on default scene
	Scene* scene = SceneManager::GetCurrentScene();
	GameObject& obj = scene->CreateObject<GameObject>();
	GameObject& obj2 = scene->CreateObject<GameObject>();

	auto tileMap = AssetManager::CreateTileMap("Tilemap1", "C:\\Users\\Ebisu\\source\\repos\\FractalEngine\\FractalEngine\\Textures\\Test.tilemap");
	tileMap->SetTileScale(5.0f, 5.0f);
	tileMap->SetTileSolid(1, true);
	tileMap->position = { 0,0 };

	// Create scene and load it
	SceneManager::CreateScene("Test2");
	SceneManager::LoadScene("Test2");

	// Create objects on "Test2" scene
	Scene* sceneTest2 = SceneManager::GetCurrentScene();
	GameObject& obj3 = sceneTest2->CreateObject<GameObject>();
	GameObject& obj4 = sceneTest2->CreateObject<GameObject>();

	// Set positions and sizes
	obj3.transform.position = { 600, 500 };
	obj4.transform.position = { 600, 50 };

	obj.transform.position = { 1600, 600 };
	obj2.transform.position = { 400, -200 };

	// Add components
	obj.AddComponent<Components::Physics2D>();
	obj2.AddComponent<Components::Physics2D>();
	obj4.AddComponent<Components::Physics2D>();

	obj.AddComponent<Components::Animator>();
	obj3.AddComponent<Components::Animator>();
	obj4.AddComponent<Components::Animator>();

	AssetManager::CreateTexture("Bob", "C:\\Users\\Ebisu\\source\\repos\\FractalEngine\\FractalEngine\\Textures\\bob.bmp");

	// Apply animation to other objects
	Components::Animator* a = obj4.GetComponent<Components::Animator>();
	a->CreateAnimation("BobAnimation", 5, 0.1f, AssetManager::GetTexture("Bob"));
	a->SetAnimation("BobAnimation");
	a->SetSpeed(0.3f);
	a->Play();

	Components::Animator* b = obj3.GetComponent<Components::Animator>();
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

	Physics::SpatialGrid::CreateCells();

	obj.transform.position = { 600, 600 };

	GameObject& obj5 = scene->CreateObject<GameObject>();
	obj5.AddComponent<Components::Sprite>();
	auto spr3 = obj5.GetComponent<Components::Sprite>();
	SDL_Color color = { 255,255,255,255 };
	spr3->texture = nullptr;
	obj5.transform.position = { 50, 50 };

	auto& light = scene->CreateObject<Light2D>();
	light.transform.position = { 500, 500 };
	light.range = 590;

	auto& light2 = scene->CreateObject<Light2D>();
	light2.transform.position = { 100, 400 };
	light2.range = 1000;

	auto& light3 = scene->CreateObject<Light2D>();
	light3.transform.position = { 1500, 500 };
	light3.range = 290;

	PrefabManager::CreatePrefab("Player", obj);

	auto player = PrefabManager::InstantiatePrefab<GameObject>("Player");

	while (FractalEngine::running)
	{
		light.transform.position = obj.transform.position;
		light2.transform.position = obj5.transform.position;
		Vector2 center{ 1500,700 };

		float speed = 0.5f;
		float radius = 800.0f;
		static float angle = 0.0f;

		angle += speed * FractalEngineCore::deltaTime;

		obj5.transform.position.x = center.x + cos(angle) * radius;
		obj5.transform.position.y = center.y + sin(angle) * radius;

		camera.follow(obj);
		FractalEngine::Run();
		if (Input::GetButtonDown(SDL_SCANCODE_1))
			FractalEngine::Stop();
		if (Input::GetButtonDown(SDL_SCANCODE_2))
			SceneManager::LoadScene("Test2");
		if (Input::GetButtonDown(SDL_SCANCODE_3))
			SceneManager::LoadScene("Default");

		if (Input::GetButtonDown(SDL_SCANCODE_N))
			for (size_t i = 0; i < 40; i++)
				PrefabManager::InstantiatePrefab<GameObject>("Player");

		if (Input::GetButtonDown(SDL_SCANCODE_0))
			FractalEngine::PrintEngineStatistics();

		if (Input::GetButtonDown(SDL_SCANCODE_F))
			light.range += 100;
		if (Input::GetButtonDown(SDL_SCANCODE_C))
			light.range -= 100;

		if (Input::GetButtonDown(SDL_SCANCODE_J))
			Rendering::Debugger::DrawSpatialPartioning = !Rendering::Debugger::DrawSpatialPartioning;

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
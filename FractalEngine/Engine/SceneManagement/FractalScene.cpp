#include "FractalScene.h"
#include "../Rendering/RenderingSystem.h"
#include "../Physics/PhysicsFunctions.h"
#include "../Core/FractalEngineCore.h"
#include <iostream>

Scene* SceneManager::currentScene = nullptr;
std::unordered_map<std::string, std::unique_ptr<Scene>> SceneManager::scenes = {};

inline SDL_Color ChangeColor(const GameObject* b)
{
	SDL_Color color;
	uint32_t id = b->ID;
	color.r = (id * 53) % 256;
	color.g = (id * 97) % 256;
	color.b = (id * 193) % 256;
	color.a = 255;
	return color;
}

void Scene::Render()
{
	Rendering::ClearScreen();
	for (auto& obj : objects)
	{
		if (obj->GetType() != Type::GAMEOBJECT) continue;

		GameObject* gameObj = static_cast<GameObject*>(obj);
		if (!gameObj) continue;

		SDL_FRect& rect = gameObj->GetRenderTarget();
		const float width = rect.w;
		const float height = rect.h;

		Components::Sprite* sprtComponent = gameObj->GetComponent<Components::Sprite>();
		SDL_Color color = (sprtComponent) ? sprtComponent->color : ChangeColor(gameObj);

		Components::Animator* animator = gameObj->GetComponent<Components::Animator>();

		if (animator)
			animator->Update(FractalEngineCore::deltaTime);

		Rendering::DrawQuad(
			gameObj->transform.position.x,
			gameObj->transform.position.y,
			width, height,
			gameObj->transform.rotation,
			sprtComponent, color);
	}
	Rendering::PushToScreen();
}

void Scene::Update()
{
	Physics::Run(objects);
}
#include "FractalScene.h"
#include "../Rendering/RenderingSystem.h"
#include "../Physics/PhysicsFunctions.h"
#include "../Core/FractalEngineCore.h"
#include <iostream>

Scene* SceneManager::currentScene = nullptr;
std::unordered_map<std::string, std::unique_ptr<Scene>> SceneManager::scenes = {};

inline SDL_Color changeColor(const GameObject* b)
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
	Rendering::clearScreen();
	for (auto& obj : objects)
	{
		if (obj->getType() != Type::GAMEOBJECT) continue;

		GameObject* gameObj = static_cast<GameObject*>(obj);
		if (!gameObj) continue;

		SDL_FRect& rect = gameObj->getRenderTarget();
		const float width = rect.w;
		const float height = rect.h;

		Components::Sprite* sprtComponent = gameObj->getComponent<Components::Sprite>();
		SDL_Color color = (sprtComponent) ? sprtComponent->color : changeColor(gameObj);

		Components::Animator* animator = gameObj->getComponent<Components::Animator>();
		if (animator)
		{
			animator->Update(FractalEngineCore::deltaTime);
			Rendering::renderAnimation(*animator);
			continue;
		}
		Rendering::drawQuad(gameObj->transform.position.x, gameObj->transform.position.y, width, height, gameObj->transform.rotation, color);
	}
	Rendering::pushToScreen();
}

void Scene::Update()
{
	Physics::Run(objects);
}
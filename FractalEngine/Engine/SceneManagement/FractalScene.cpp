#include "FractalScene.h"
#include "../Rendering/RenderingSystem.h"
#include "../Physics/PhysicsFunctions.h"
#include <iostream>

Scene* SceneManager::currentScene = nullptr;
std::unordered_map<std::string, std::unique_ptr<Scene>> SceneManager::scenes = {};

void Scene::Render()
{
	Rendering::clearScreen();
	for (auto& obj : objects)
	{
		GameObject* b = dynamic_cast<GameObject*>(obj);
		if (!b) continue;

		Components::Sprite* sprtComponent = b->getComponent<Components::Sprite>();

		const float width = (sprtComponent) ? sprtComponent->width : 150;
		const float height = (sprtComponent) ? sprtComponent->height : 150;

		SDL_Color color;
		if (sprtComponent)
		{
			color = sprtComponent->color;
		}
		else
		{
			uint32_t id = b->ID;
			color.r = (id * 53) % 256;
			color.g = (id * 97) % 256;
			color.b = (id * 193) % 256;
			color.a = 255;
		}

		Rendering::drawQuad(b->transform.position.x, b->transform.position.y, width, height, b->transform.rotation, color);
	}
	Rendering::pushToScreen();
}

void Scene::Update()
{
	Physics::Run(objects);
}
#include "FractalScene.h"
#include "../Rendering/RenderingSystem.h"
#include "../Physics/PhysicsFunctions.h"

Scene* SceneManager::currentScene = nullptr;
std::unordered_map<std::string, std::unique_ptr<Scene>> SceneManager::scenes = {};

void Scene::Render()
{
	Rendering::drawScreen(this);
	Rendering::clearScreen();
}

void Scene::Update()
{
	Physics::Run(objects);
}
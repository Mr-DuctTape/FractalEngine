#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

class Object;
#include "../EntitySystem/Entities.h"

class Scene
{
public:
	std::vector<Object*> objects = {};
	std::string name;
	void Render();
	void Update();
	Scene(const std::string sceneName)
	{
		name = sceneName;
	};
	~Scene()
	{
		for (auto& obj : objects)
		{
			delete obj;
		}
	}
};

class SceneManager
{
private:
	static Scene* currentScene;
	static std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
public:
	static void createScene(const std::string& name)
	{
		SceneManager::scenes.emplace(name, std::make_unique<Scene>(name));
		if (!currentScene)
			loadScene(name);
	}
	static void loadScene(const std::string& name)
	{
		for (auto& s : scenes)
		{
			if (s.first == name)
				currentScene = s.second.get();
		}
	}
	static Scene* getScene(const std::string& name)
	{
		auto scene = SceneManager::scenes.find(name);
		return scene->second.get();
	}
	static void createDefault()
	{
		createScene("Default");
	}
	inline static Scene* getCurrentScene() { return currentScene; };
	static void runCurrentScene()
	{
		if (!currentScene)
		{
			createDefault();
			return;
		}

		currentScene->Update();
		currentScene->Render();
	}
};
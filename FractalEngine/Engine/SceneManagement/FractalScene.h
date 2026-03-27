#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

class Object;
#include "../EntitySystem/Entities.h"

class Scene
{
private:
	std::string name;
public:
	std::vector<Object*> objects = {};
	void Render();
	void Update();
	Scene(const std::string sceneName)
	{
		name = sceneName;
	};
	inline std::string getName() const
	{
		return name;
	}
	~Scene()
	{
		for (auto obj : objects)
			delete obj;
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
		if (!SceneManager::currentScene)
			loadScene(name);
	}
	static void loadScene(const std::string& name)
	{
		auto it = SceneManager::scenes.find(name);
		if (it != SceneManager::scenes.end())
			SceneManager::currentScene = it->second.get();
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
	static void loadDefault()
	{
		bool defualt_exist = (SceneManager::scenes.find("Default") != SceneManager::scenes.end()) ? true : false;
	
		if (defualt_exist)
			loadScene("Default");
		else
		{
			createDefault();
			loadScene("Default");
		}
	}
	inline static Scene* getCurrentScene() { return SceneManager::currentScene; };
};
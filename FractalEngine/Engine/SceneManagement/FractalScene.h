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
	inline std::string GetName() const
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
	static void CreateScene(const std::string& name)
	{
		SceneManager::scenes.emplace(name, std::make_unique<Scene>(name));
		if (!SceneManager::currentScene)
			LoadScene(name);
	}
	static void LoadScene(const std::string& name)
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
	static void CreateDefault()
	{
		CreateScene("Default");
	}
	static void LoadDefault()
	{
		bool default_exists = (SceneManager::scenes.find("Default") != SceneManager::scenes.end()) ? true : false;
	
		if (default_exists)
			LoadScene("Default");
		else
		{
			CreateDefault();
			LoadScene("Default");
		}
	}
	inline static Scene* GetCurrentScene() { return SceneManager::currentScene; };
};
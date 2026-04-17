#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

class Object;

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
	template <typename T> //Creates object on the current active scene
	T& CreateObject()
	{
		auto location = new T();
		objects.push_back(location);
		return *location;
	}
	template <typename T>
	T& CreateObject(const T& value)
	{
		auto location = new T(value); 
		objects.push_back(location);
		return *location;
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
	static void LoadScene(const std::string& name);
	static Scene* GetScene(const std::string& name)
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
#pragma once
#include "Entities.h"
#include "../SceneManagement/FractalScene.h"
#include <unordered_map>

class PrefabManager
{
private:
	static std::unordered_map<std::string, std::unique_ptr<Object>> prefabs;

public:
	static void CreatePrefab(const std::string name, const Object& object);
	static void DestroyPrefab(const std::string name);

	template<typename T>
	static T* InstantiatePrefab(const std::string name)
	{
		auto it = prefabs.find(name);
		if (it == prefabs.end()) return nullptr;

		Scene* scene = SceneManager::GetCurrentScene();
		if (!scene) return nullptr;

		Object* clone = it->second->Clone();
		scene->objects.push_back(clone);

		T* obj = dynamic_cast<T*>(clone);
		return obj;
	}
};
#include "PrefabManager.h"

std::unordered_map<std::string, std::unique_ptr<Object>> PrefabManager::prefabs;

void PrefabManager::CreatePrefab(const std::string name, const Object& object)
{
	prefabs.emplace(name, std::unique_ptr<Object>(object.Clone()));
}

void PrefabManager::DestroyPrefab(const std::string name)
{
	prefabs.erase(name);
}
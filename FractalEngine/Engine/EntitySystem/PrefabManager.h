#pragma once
#include "Entities.h"
#include <unordered_map>

class PrefabManager
{
private:
	static std::unordered_map<std::string, Object> ObjectPrefabs;
public:
	template <typename T>
	void CreatePrefab(const std::string& name, const T& object)
	{
		ObjectPrefabs.emplace(name, object);
	}
};
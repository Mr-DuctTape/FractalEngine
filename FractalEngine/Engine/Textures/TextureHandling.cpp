#include "TextureHandling.h"
#include "../Core/FractalEngineCore.h"
#include <iostream>

std::unordered_map<std::string, SDL_Texture*> AssetManager::textures;

SDL_Texture* AssetManager::CreateTextureBMP(const char* path)
{
	SDL_Surface* imageSpecs = SDL_LoadBMP(path);

	if (!imageSpecs)
	{
		std::cout << SDL_GetError() << std::endl;
		return nullptr;
	}
	else
		return nullptr;
}

SDL_Texture* AssetManager::CreateTexture(const std::string& name, const std::string& path)
{
	SDL_Texture* texture = AssetManager::CreateTextureBMP(path.c_str());
	AssetManager::textures.emplace(name, texture);
	return texture;
}
SDL_Texture* AssetManager::GetTexture(const std::string& name)
{
	auto it = AssetManager::textures.find(name);
	if (it != AssetManager::textures.end())
		return it->second;
	return nullptr;
}
SDL_Texture* AssetManager::GetTexture(const unsigned int index)
{
	unsigned int indexer = 0;
	for (auto& texture : AssetManager::textures)
		if (indexer++ == index)
			return texture.second;
	return nullptr;
}
void AssetManager::Clear()
{
	for (auto it = AssetManager::textures.begin(); it != AssetManager::textures.end(); ++it)
	{
		if (it->second)
			SDL_DestroyTexture(it->second);
	}
	AssetManager::textures.clear();
}
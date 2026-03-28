#include "TextureHandling.h"
#include "../Core/FractalEngineCore.h"
#include "../Rendering/RenderingSystem.h"
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

	SDL_Texture* texture = SDL_CreateTextureFromSurface(Rendering::getRenderer(), imageSpecs);
	return texture;
}

SDL_Texture* AssetManager::CreateTexture(const std::string& name, const std::string& path)
{
	SDL_Texture* texture = AssetManager::CreateTextureBMP(path.c_str());
	AssetManager::textures.emplace(name, texture);
	if (!texture)
		std::cout << "Failed to create texture!: " << name << "\n";
	return texture;
}
SDL_Texture* AssetManager::GetTexture(const std::string& name)
{
	auto it = AssetManager::textures.find(name);
	if (it != AssetManager::textures.end())
		return it->second;
	std::cout << "Failed to get texture!: " << name << "\n";
	return nullptr;
}
SDL_Texture* AssetManager::GetTexture(const unsigned int index)
{
	unsigned int indexer = 0;
	for (auto& texture : AssetManager::textures)
		if (indexer++ == index)
			return texture.second;
	std::cout << "Failed to get texture!: " << index << "\n";
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
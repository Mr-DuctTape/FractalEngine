#include "TextureHandling.h"
#include "../Core/FractalEngineCore.h"
#include "../Rendering/RenderingSystem.h"
#include "../EntitySystem/Entities.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::unordered_map<std::string, SDL_Texture*> AssetManager::textures;

std::vector<AssetManager::Map> AssetManager::tileMaps;

SDL_Color AssetManager::_removeColor = {255, 255, 255};

SDL_Texture* AssetManager::CreateTextureBMP(const char* path)
{
	SDL_Surface* imageSpecs = SDL_LoadBMP(path);

	if (!imageSpecs)
	{
		std::cout << SDL_GetError() << std::endl;
		return nullptr;
	}

	uint8_t* pixels = static_cast<uint8_t*>(imageSpecs->pixels);
	int bpp = SDL_BYTESPERPIXEL(imageSpecs->format);

	for (int y = 0; y < imageSpecs->h; y++)
	{
		for (int x = 0; x < imageSpecs->w; x++)
		{
			uint8_t* pixel = pixels + y * imageSpecs->pitch + x * bpp;

			uint8_t* b = &pixel[0];
			uint8_t* g = &pixel[1];
			uint8_t* r = &pixel[2];

			if (*r == _removeColor.r)
				*r = 0;
			if (*g == _removeColor.g)
				*g = 0;
			if (*b == _removeColor.b)
				*b = 0;
		}
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(Rendering::GetRenderer(), imageSpecs);
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	SDL_DestroySurface(imageSpecs);
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

TileMap* AssetManager::CreateTileMap(const std::string& name, const std::string& filePath)
{
	std::ifstream file(filePath.c_str());

	if (!file.is_open())
	{
		std::cout << "File not found\n";
		return nullptr;
	}

	std::string tileSpritePath;
	std::string line;
	unsigned int pixelWidth = 0, pixelHeight = 0;
	bool readingMap = false;

	TileMap& tileMap = CreateObject<TileMap>();

	while (std::getline(file, line))
	{
		if (line.empty()) continue;

		if (readingMap)
		{
			std::stringstream ss(line);

			std::vector<unsigned int> row;
			unsigned int tile;
			for (char c : line)
			{
				if (std::isdigit(c))
				{
					row.push_back(c - '0'); 
				}
			}
			if (!row.empty())
			{
				tileMap.GetTiles().push_back(row);
			}
			continue;
		}

		if (line.rfind("Tile_size:", 0) == 0)
		{
			size_t pos = line.find(":");
			std::string data = line.substr(pos + 1);
			std::stringstream ss(data);
			ss >> pixelWidth >> pixelHeight;
		}
		else if (line.rfind("Sprite_Sheet:", 0) == 0)
		{
			size_t pos = line.find(":");
			std::string data = line.substr(pos + 1);
			std::stringstream ss(data);
			ss >> tileSpritePath;
		}
		else if (line.rfind("Map:", 0) == 0)
		{
			readingMap = true;
			continue;
		}
	}

	SDL_Texture* texture = AssetManager::CreateTexture(name + " tile_sheet", tileSpritePath);
	int tiles = texture->w / pixelWidth;
	tileMap.SetTilePixels(pixelWidth, pixelHeight);
	tileMap.SetTileSet(texture, tiles);

	AssetManager::Map temp;
	temp.name = name;
	temp.map = &tileMap;
	AssetManager::tileMaps.push_back(temp);

	return &tileMap;
}

TileMap* AssetManager::GetTileMap(const std::string& name)
{
	for (auto& it : AssetManager::tileMaps)
	{
		if (it.name == name)
		{
			return it.map;
		}
	}
	return nullptr;
}
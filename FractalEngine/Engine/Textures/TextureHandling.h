#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <string>
#include "../EntitySystem/Entities.h"

class AssetManager
{
private:
	static std::unordered_map<std::string, SDL_Texture*> textures;
	static SDL_Texture* CreateTextureBMP(const char* path);
	static SDL_Color _removeColor;
	struct Map
	{
		std::string name = {};
		TileMap* map = nullptr;
	};
	static std::vector<Map> tileMaps;
public:
	static SDL_Texture* CreateTexture(const std::string& name, const std::string& path);
	static SDL_Texture* GetTexture(const std::string& name);
	static SDL_Texture* GetTexture(const unsigned int index);

	// Must be of custom format .tilemap
	// Creates a tilemap on the CURRENTLY ACTIVE SCENE!
	static TileMap* CreateTileMap(const std::string& name, const std::string& filePath); 
	static TileMap* GetTileMap(const std::string& name);

	static void Clear();
};
#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <string>

class AssetManager
{
private:
	static std::unordered_map<std::string, SDL_Texture*> textures;
	static SDL_Texture* CreateTextureBMP(const char* path);
	static SDL_Color _removeColor;

public:
	static SDL_Texture* CreateTexture(const std::string& name, const std::string& path);
	static SDL_Texture* GetTexture(const std::string& name);
	static SDL_Texture* GetTexture(const unsigned int index);
	static void Clear();
};
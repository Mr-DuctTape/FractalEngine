#include "TextureHandling.h"
#include "../FractalEngine.h"
#include <iostream>

SDL_Texture* Textures::CreateTextureBMP(const char* path)
{
	SDL_Surface* imageSpecs = SDL_LoadBMP(path);

	if (!imageSpecs)
	{
		std::cout << SDL_GetError() << std::endl;
		return nullptr;
	}
	else
		return SDL_CreateTextureFromSurface(FractalEngine::renderer, imageSpecs);
}
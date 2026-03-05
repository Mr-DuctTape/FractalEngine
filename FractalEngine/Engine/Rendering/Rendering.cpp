#include "RenderingSystem.h"
#include "../FractalEngine.h"

std::vector<int> Rendering::indices = {};
std::vector<SDL_Vertex> Rendering::vertices = {};

void Rendering::clearScreen()
{
	SDL_Color preColor;
	SDL_GetRenderDrawColor(FractalEngine::renderer, &preColor.r, &preColor.g, &preColor.b, &preColor.a);
	SDL_Color color{ 0,0,0,255 };
	SDL_SetRenderDrawColor(FractalEngine::renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(FractalEngine::renderer);
	SDL_SetRenderDrawColor(FractalEngine::renderer, preColor.r, preColor.g, preColor.b, preColor.a);
}

void Rendering::clearScreen(SDL_Color color)
{
	SDL_Color preColor;
	SDL_GetRenderDrawColor(FractalEngine::renderer, &preColor.r, &preColor.g, &preColor.b, &preColor.a);
	SDL_SetRenderDrawColor(FractalEngine::renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(FractalEngine::renderer);
	SDL_SetRenderDrawColor(FractalEngine::renderer, preColor.r, preColor.g, preColor.b, preColor.a);
}


void Rendering::drawScreen()
{

}
#include "RenderingSystem.h"
#include "../FractalEngine.h"
#include "../EntitySystem/Entities.h"
#include <iostream>

std::vector<int> indices = {};
std::vector<SDL_Vertex> vertices = {};

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
	//Doing lazy drawing, gonna make it into indicies and vertices later for performance
	for (const auto &obj : objects)
	{
		Components::Sprite* sprite = nullptr;
		auto* c = obj.get();
		const bool hasSprite = c->hasComponent<Components::Sprite>();

		SDL_FRect rect;
		if (hasSprite)
		{
			sprite = c->getComponent<Components::Sprite>();
			rect.w = sprite->width;
			rect.h = sprite->height;
		}
		else
		{
			rect.w = 150;
			rect.h = 150;
		}

		rect.x = c->transform.position.x;
		rect.y = c->transform.position.y;

		if (hasSprite)
			SDL_RenderTexture(FractalEngine::renderer, sprite->texture, NULL, &rect);
		else
		{
			SDL_SetRenderDrawColor(FractalEngine::renderer, 255, 255, 255, 255);
			SDL_RenderRect(FractalEngine::renderer, &rect);
			SDL_SetRenderDrawColor(FractalEngine::renderer, 0, 0, 0, 0);
		}
	}

	SDL_RenderPresent(FractalEngine::renderer);
}
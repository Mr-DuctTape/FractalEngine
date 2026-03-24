#include "RenderingSystem.h"
#include "../Core/FractalEngineCore.h"
#include "../EntitySystem/Entities.h"
#include "../SceneManagement/FractalScene.h"
#include <iostream>

std::vector<int> indices = {};
std::vector<SDL_Vertex> vertices = {};

void Rendering::clearScreen()
{
	SDL_Color preColor;
	SDL_GetRenderDrawColor(FractalEngineCore::renderer, &preColor.r, &preColor.g, &preColor.b, &preColor.a);
	SDL_Color color{ 0,0,0,255 };
	SDL_SetRenderDrawColor(FractalEngineCore::renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(FractalEngineCore::renderer);
	SDL_SetRenderDrawColor(FractalEngineCore::renderer, preColor.r, preColor.g, preColor.b, preColor.a);
}

void Rendering::clearScreen(SDL_Color color)
{
	SDL_Color preColor;
	SDL_GetRenderDrawColor(FractalEngineCore::renderer, &preColor.r, &preColor.g, &preColor.b, &preColor.a);
	SDL_SetRenderDrawColor(FractalEngineCore::renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(FractalEngineCore::renderer);
	SDL_SetRenderDrawColor(FractalEngineCore::renderer, preColor.r, preColor.g, preColor.b, preColor.a);
}

void Rendering::drawScreen(Scene* scene)
{
	//Doing lazy drawing, gonna make it into indicies and vertices later for performance
	for (const auto &b : scene->objects)
	{
		GameObject* c = dynamic_cast<GameObject*>(b);
		if (!c)
			return;

		Components::Sprite* sprite = nullptr;
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

		rect.x = c->transform.position.x - camera.transform.position.x;
		rect.y = c->transform.position.y - camera.transform.position.y;

		if (hasSprite)
			SDL_RenderTexture(FractalEngineCore::renderer, sprite->texture, NULL, &rect);
		else
		{
			SDL_SetRenderDrawColor(FractalEngineCore::renderer, 255, 255, 255, 255);
			SDL_RenderRect(FractalEngineCore::renderer, &rect);
			SDL_SetRenderDrawColor(FractalEngineCore::renderer, 0, 0, 0, 0);
		}
	}

	SDL_RenderPresent(FractalEngineCore::renderer);
}
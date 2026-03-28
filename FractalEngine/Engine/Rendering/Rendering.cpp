#include "RenderingSystem.h"
#include "../Core/FractalEngineCore.h"
#include "../EntitySystem/Entities.h"
#include "../SceneManagement/FractalScene.h"
#include <iostream>

float Rendering::cosTable[360] = {};
float Rendering::sinTable[360] = {};

SDL_Renderer* Rendering::renderer = nullptr;

std::vector<Rendering::Line> Rendering::Lines = {};
std::vector<int> Rendering::Indices = {};
std::vector<SDL_Vertex> Rendering::Vertices = {};

SDL_FColor toFColor(const SDL_Color& c)
{
	return SDL_FColor{
		c.r / 255.0f,
		c.g / 255.0f,
		c.b / 255.0f,
		c.a / 255.0f
	};
}

void Rendering::clearScreen(SDL_Color color)
{
	SDL_Color preColor;
	SDL_GetRenderDrawColor(Rendering::renderer, &preColor.r, &preColor.g, &preColor.b, &preColor.a);
	SDL_SetRenderDrawColor(Rendering::renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(Rendering::renderer);
	SDL_SetRenderDrawColor(Rendering::renderer, preColor.r, preColor.g, preColor.b, preColor.a);
}

void Rendering::drawLine(float x1, float y1, float x2, float y2, SDL_Color color)
{
	Rendering::Lines.emplace_back(Line{ x1, y1, x2, y2, color });
}

void Rendering::drawQuad(float x, float y, float w, float h, float rotation, const SDL_Color& color)
{
	SDL_FColor quadColor = toFColor(color);

	SDL_Vertex vertices[4] =
	{
		{x, y},
		{ x + w, y },
		{ x, y + h },
		{ x + w, y + h }
	};

	const int index = static_cast<int>(rotation) % 360;
	const float cos = cosTable[index];
	const float sin = sinTable[index];
	SDL_FPoint center = { w / 2.0 + x,  h / 2.0 + y };

	const int startIndex = Rendering::Vertices.size();
	int indices[6] =
	{
		0 + startIndex,1 + startIndex,2 + startIndex, //First triangle
		2 + startIndex,1 + startIndex,3 + startIndex //Second triangle
	};
	for (size_t i = 0; i < 6; i++)
	{
		Rendering::Indices.push_back(indices[i]);
	}

	for (size_t i = 0; i < 4; i++)
	{
		//Rotation
		float vx = vertices[i].position.x;
		float vy = vertices[i].position.y;

		vertices[i].position.x = (vx - center.x) * cos - (vy - center.y) * sin + center.x;
		vertices[i].position.y = (vx - center.x) * sin + (vy - center.y) * cos + center.y;
		//

		vertices[i].color = quadColor;
		Rendering::Vertices.push_back(vertices[i]);
	}
}

void Rendering::pushToScreen()
{
	SDL_RenderGeometry(
		Rendering::getRenderer(),
		NULL, //Texture to use
		Rendering::Vertices.data(),
		Rendering::Vertices.size(),
		Rendering::Indices.data(),
		Rendering::Indices.size());

	SDL_Color color;
	SDL_GetRenderDrawColor(Rendering::renderer, &color.r, &color.g, &color.b, &color.a);

	if(!Lines.empty())
		for (size_t i = 0; i < Lines.size(); i++)
		{
			SDL_SetRenderDrawColor(Rendering::renderer, Lines[i].color.r, Lines[i].color.g, Lines[i].color.b, Lines[i].color.a);
			SDL_RenderLine(Rendering::renderer, Lines[i].x1, Lines[i].y1, Lines[i].x2, Lines[i].y2);
		}

	SDL_SetRenderDrawColor(Rendering::renderer, color.r, color.g, color.b, color.a);
	SDL_RenderPresent(Rendering::renderer);

	//only clears the size not capacity so it doesnt shrink the actual memory
	Rendering::Vertices.clear();
	Rendering::Indices.clear();
}

void Rendering::renderAnimation(const Components::Animator& animator)
{
	if (!animator.currentAnimation) return;

	auto* currentAnimation = animator.currentAnimation;

	SDL_FRect srcRect = currentAnimation->frame;
	currentAnimation->xOffset = currentAnimation->frame.w * currentAnimation->frameIndex;

	if (currentAnimation->xOffset >= currentAnimation->spriteSheet->w)
	{
		currentAnimation->yOffset += currentAnimation->spriteSheet->h / currentAnimation->numberOfFrames;
	}

	srcRect.x = currentAnimation->xOffset;
	srcRect.y = currentAnimation->yOffset;

	SDL_RenderTexture(Rendering::getRenderer(), 
		currentAnimation->spriteSheet,
		&srcRect, 
		currentAnimation->renderTarget);
}
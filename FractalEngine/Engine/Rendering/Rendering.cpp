#include "RenderingSystem.h"
#include "../Core/FractalEngineCore.h"
#include "../EntitySystem/Entities.h"
#include "../SceneManagement/FractalScene.h"
#include <iostream>

float Rendering::cosTable[360] = {};
float Rendering::sinTable[360] = {};

int Rendering::Batch::batchNumber = 0;

SDL_Renderer* Rendering::_Renderer = nullptr;

std::vector<std::unique_ptr<Rendering::Batch>> Rendering::_Batches = {};

std::vector<Rendering::Line> Rendering::_Lines = {};
std::vector<SDL_Vertex> Rendering::_Vertices = {};
std::vector<int> Rendering::_Indices = {};

SDL_FColor toFColor(const SDL_Color& c)
{
	return SDL_FColor{
		c.r / 255.0f,
		c.g / 255.0f,
		c.b / 255.0f,
		c.a / 255.0f
	};
}

void Rendering::ClearScreen(SDL_Color color)
{
	SDL_Color preColor;
	SDL_GetRenderDrawColor(Rendering::_Renderer, &preColor.r, &preColor.g, &preColor.b, &preColor.a);
	SDL_SetRenderDrawColor(Rendering::_Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(Rendering::_Renderer);
	SDL_SetRenderDrawColor(Rendering::_Renderer, preColor.r, preColor.g, preColor.b, preColor.a);
}

void Rendering::DrawLine(float x1, float v1, float x2, float v2, SDL_Color color)
{
	Rendering::_Lines.emplace_back(Line{ x1, v1, x2, v2, color });
}

Rendering::Batch* Rendering::CreateBatch(SDL_Texture* texture)
{
	auto batch = std::make_unique<Batch>();
	auto ptr = batch.get();

	if (texture)
		ptr->texture = texture;
	else
		ptr->texture = nullptr;

	_Batches.push_back(std::move(batch));
	return ptr;
}

void Rendering::DrawQuad(float x, float y, float w, float h, float rotation, Components::Sprite* sprite, const SDL_Color& color)
{
	Rendering::Batch* currentBatch = nullptr;
	for (auto& batch : _Batches)
	{
		if (sprite)
		{
			if (batch.get()->texture == sprite->texture)
			{
				currentBatch = batch.get();
				break;
			}
		}
		else
		{
			if (batch.get()->texture == nullptr)
			{
				currentBatch = batch.get();
			}
		}
	}

	if (currentBatch == nullptr && sprite)
		currentBatch = Rendering::CreateBatch(sprite->texture);
	else if(currentBatch == nullptr && !sprite)
		currentBatch = Rendering::CreateBatch(nullptr);

	SDL_FColor quadColor = toFColor(color);
	SDL_Vertex vertices[4];

	vertices[0].position = { x, y };
	vertices[1].position = { x + w, y };
	vertices[2].position = { x, y + h };
	vertices[3].position = { x + w, y + h };

	if (sprite)
	{
		float u1 = sprite->u1; // left
		float u2 = sprite->u2; // right
		float v1 = sprite->v1; // top
		float v2 = sprite->v2; // botto

		vertices[0].tex_coord = { u1, v1 }; // top-left
		vertices[1].tex_coord = { u2, v1 }; // top-right
		vertices[2].tex_coord = { u1, v2 }; // bottom-left
		vertices[3].tex_coord = { u2, v2 }; // bottom-right
	}

	const int startIndex = currentBatch->_Vertices.size();
	int indices[6] =
	{
		0 + startIndex,1 + startIndex,2 + startIndex, //First triangle
		2 + startIndex,1 + startIndex,3 + startIndex //Second triangle
	};
	for (size_t i = 0; i < 6; i++)
	{
		currentBatch->_Indices.push_back(indices[i]);
	}

	const int index = static_cast<int>(rotation) % 360;
	const float cos = cosTable[index];
	const float sin = sinTable[index];
	const SDL_FPoint center = { w / 2.0 + x,  h / 2.0 + y };

	for (size_t i = 0; i < 4; i++)
	{
		//Rotation
		float vx = vertices[i].position.x;
		float vy = vertices[i].position.y;

		vertices[i].position.x = (vx - center.x) * cos - (vy - center.y) * sin + center.x;
		vertices[i].position.y = (vx - center.x) * sin + (vy - center.y) * cos + center.y;
		//

		vertices[i].color = quadColor;
		currentBatch->_Vertices.push_back(vertices[i]);
	}
}

void Rendering::PushToScreen()
{
	for (auto& b : _Batches)
	{
		auto currentBatch = b.get();

		if (currentBatch->texture == nullptr)
		{
			SDL_RenderGeometry(
				Rendering::GetRenderer(),
				NULL, //Texture to use
				currentBatch->_Vertices.data(),
				currentBatch->_Vertices.size(),
				currentBatch->_Indices.data(),
				currentBatch->_Indices.size());
		}
		else
		{
			SDL_RenderGeometry(
				Rendering::GetRenderer(),
				currentBatch->texture, //Texture to use
				currentBatch->_Vertices.data(),
				currentBatch->_Vertices.size(),
				currentBatch->_Indices.data(),
				currentBatch->_Indices.size());
		}
		currentBatch->_Vertices.clear();
		currentBatch->_Indices.clear();
	}

	SDL_Color color;
	SDL_GetRenderDrawColor(Rendering::_Renderer, &color.r, &color.g, &color.b, &color.a);

	SDL_SetRenderDrawColor(Rendering::_Renderer, 255, 0, 0, 255);
	SDL_RenderLine(Rendering::_Renderer, 600, 0, 600 + 150, 150);

	if (!_Lines.empty())
		for (size_t i = 0; i < _Lines.size(); i++)
		{
			SDL_SetRenderDrawColor(Rendering::_Renderer, _Lines[i].color.r, _Lines[i].color.g, _Lines[i].color.b, _Lines[i].color.a);
			SDL_RenderLine(Rendering::_Renderer, _Lines[i].x1, _Lines[i].v1, _Lines[i].x2, _Lines[i].v2);
		}

	SDL_SetRenderDrawColor(Rendering::_Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderPresent(Rendering::_Renderer);
}
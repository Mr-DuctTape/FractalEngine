#include "RenderingSystem.h"
#include "../Core/FractalEngineCore.h"
#include "../EntitySystem/Entities.h"
#include "../SceneManagement/FractalScene.h"
#include <iostream>
#include <algorithm>

float Rendering::cosTable[360] = {};
float Rendering::sinTable[360] = {};

int Rendering::Batch::batchNumber = 0;
SDL_Renderer* Rendering::_Renderer = nullptr;
std::vector<std::unique_ptr<Rendering::Batch>> Rendering::_Batches = {};
std::vector<Rendering::Line> Rendering::_Lines = {};


std::vector<Rendering::Debug::Box> Rendering::Debug::collisionBoxes = {};

void Rendering::Debug::DrawCollisionBox(const Components::CollisionBox& box, const SDL_Color& color, bool solid)
{
	SDL_FRect rect;
	rect.x = box.minX - camera.position.x;
	rect.y = box.minY - camera.position.y;
	rect.w = box.maxX - box.minX;
	rect.h = box.maxY - box.minY;

	Rendering::Debug::Box a;
	a.rect = rect;
	a.color = color;
	a.solid = solid;

	collisionBoxes.push_back(a);
}

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
	for (const auto& line : _Lines )
	{
		if (line.x1 == x1 && line.x2 == x2 && line.v1 == v1 && line.v2 == v2)
			return;
	}
	Rendering::_Lines.emplace_back(Line{ x1, v1, x2, v2, color });
}

Rendering::Batch* Rendering::_CreateBatch(SDL_Texture* texture)
{
	auto batch = std::make_unique<Batch>();
	auto ptr = batch.get();
	ptr->scene = SceneManager::GetCurrentScene();
	ptr->texture = (texture) ? texture : nullptr;
	_Batches.push_back(std::move(batch));
	return ptr;
}

Rendering::Batch* Rendering::_FindBatch(SDL_Texture* texture)
{
	for (size_t i = 0; i < _Batches.size(); i++)
	{
		Rendering::Batch* obj = _Batches[i].get();
		if (obj->texture == texture)
			return obj;
	}
	return nullptr;
}

Rendering::Batch* Rendering::_FindOrCreateBatch(Components::Sprite* sprite)
{
	Rendering::Batch* currentBatch = _FindBatch((sprite) ? sprite->texture : nullptr);

	if (!currentBatch)
		currentBatch = _CreateBatch((sprite) ? sprite->texture : nullptr);

	return currentBatch;
}

void Rendering::DrawQuad(float x, float y, float w, float h, float rotation, Components::Sprite* sprite, const SDL_Color& color)
{
	Rendering::Batch* currentBatch = Rendering::_FindOrCreateBatch(sprite);

	SDL_FColor quadColor = toFColor(color);

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

	float posX = x - camera.position.x;
	float posY = y - camera.position.y;
	const SDL_FPoint center = { w / 2.0 + posX,  h / 2.0 + posY};
	SDL_Vertex vertices[4];

	vertices[0].position = { posX, posY };
	vertices[1].position = { posX + w, posY };
	vertices[2].position = { posX, posY + h };
	vertices[3].position = { posX + w, posY + h };

	if (sprite)
	{
		float u1 = sprite->u1; // left
		float u2 = sprite->u2; // right
		float v1 = sprite->v1; // top
		float v2 = sprite->v2; // botto

		if (sprite->flippedX)
		{
			float temp = u1;
			u1 = u2;
			u2 = temp;
		}

		vertices[0].tex_coord = { u1, v1 }; // top-left
		vertices[1].tex_coord = { u2, v1 }; // top-right
		vertices[2].tex_coord = { u1, v2 }; // bottom-left
		vertices[3].tex_coord = { u2, v2 }; // bottom-right
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
		currentBatch->_Vertices.push_back(vertices[i]);
	}
}

void Rendering::PushToScreen()
{
	if (!Rendering::_Renderer)
		return;

	SDL_Color color;
	SDL_GetRenderDrawColor(Rendering::_Renderer, &color.r, &color.g, &color.b, &color.a);

	for (auto& box : Debug::GetCollisionBoxes())
	{
		SDL_SetRenderDrawColor(Rendering::GetRenderer(), box.color.r, box.color.g, box.color.b, box.color.a);
		if (box.solid)
			SDL_RenderFillRect(Rendering::GetRenderer(), &box.rect);
		else
			SDL_RenderRect(Rendering::GetRenderer(), &box.rect);

		box.lifeTime++;
	}

	auto& boxes = Debug::GetCollisionBoxes();

	boxes.clear();

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
		else if (currentBatch->texture)
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

	if (!_Lines.empty())
		for (size_t i = 0; i < _Lines.size(); i++)
		{
			SDL_SetRenderDrawColor(Rendering::_Renderer, _Lines[i].color.r, _Lines[i].color.g, _Lines[i].color.b, _Lines[i].color.a);
			SDL_RenderLine(Rendering::_Renderer, _Lines[i].x1, _Lines[i].v1, _Lines[i].x2, _Lines[i].v2);
		}
	_Lines.clear();

	SDL_SetRenderDrawColor(Rendering::_Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderPresent(Rendering::_Renderer);
}
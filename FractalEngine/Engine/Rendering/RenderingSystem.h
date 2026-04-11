#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include "../EntitySystem/Entities.h"
#include <memory>

class Scene;

class Rendering
{
public:
	class Debug
	{
	private:
		struct Box
		{
			SDL_FRect rect = {};
			SDL_Color color = {};
			bool solid = false;
		};
		static std::vector<Box> collisionBoxes;
	public:
		static void DrawCollisionBox(const Components::Collider2D::CollisionBox& box, const SDL_Color& color, bool solid);
		static std::vector<Box>& GetCollisionBoxes()
		{
			return collisionBoxes;
		}
	};
	struct Batch
	{
		static int batchNumber;
		std::vector<SDL_Vertex> _Vertices = {};
		std::vector<int> _Indices = {};
		SDL_Texture* texture = nullptr;
		Scene* scene = nullptr;
		Batch()
		{
			_Indices.reserve(2500);
			_Vertices.reserve(2000);
			++batchNumber;
		}
	};
private:
	static std::vector<std::unique_ptr<Batch>> _Batches;
	static Rendering::Batch* _FindBatch(SDL_Texture* texture);
	static Rendering::Batch* _CreateBatch(SDL_Texture* texture);
	static Rendering::Batch* _FindOrCreateBatch(Components::Sprite * sprite);

	//Figure out better way to draw lines? Batching?
	struct Line
	{
		float x1;
		float v1;
		float x2;
		float v2;
		SDL_Color color;
	};
	static std::vector<Line> _Lines;

	static SDL_Renderer* _Renderer;
	static float cosTable[360];
	static float sinTable[360];
	static void calculateTables()
	{
		static bool calculated = false;
		if (calculated) return;
		for (size_t i = 0; i < 360; i++)
		{
			cosTable[i] = cos(static_cast<float>(i) * 3.14159265f / 180.0);
			sinTable[i] = sin(static_cast<float>(i) * 3.14159265f / 180.0);
		}
		calculated = true;
	}
public:
	static void Init(SDL_Window* window)
	{
		if (!_Renderer)
		{
			_Lines.reserve(1000);
			_Batches.reserve(1000);
			calculateTables();
			_Renderer = SDL_CreateRenderer(window, NULL);
			SDL_SetRenderDrawBlendMode(_Renderer, SDL_BLENDMODE_BLEND);
		}
	}
	static SDL_Renderer* GetRenderer()
	{
		return _Renderer;
	}
	static void ClearScreen(SDL_Color color = { 0, 0, 0, 0 });
	static void DrawQuad(float x, float y, float w, float h, float rotation, Components::Sprite* sprite, const SDL_Color& color);
	static void DrawLine(float x1, float v1, float x2, float v2, SDL_Color color);
	static void PushToScreen();
};
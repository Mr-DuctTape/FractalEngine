#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include "../EntitySystem/Entities.h"
#include <memory>

class Scene;

class Rendering
{
private:
	struct Batch
	{
		static int batchNumber;
		SDL_Texture* texture = nullptr;
		std::vector<int> _Indices = {};
		std::vector<SDL_Vertex> _Vertices = {};
		Batch()
		{
			std::cout << "Created batch: " << ++batchNumber << "\n";
		}
	};
	static std::vector<std::unique_ptr<Batch>> _Batches;
	static Batch* _GetBatch(SDL_Texture* texture)
	{ 
		if (!texture) return _Batches[0].get();
		for (size_t i = 0; i < _Batches.size(); i++)
		{
			auto obj = _Batches[i].get();
			if (obj->texture)
				if (obj->texture == texture)
					return obj;
		}
	}
	static Batch* _FindBatch(SDL_Texture* texture)
	{
		if (!texture) return _Batches[0].get();
		for (size_t i = 0; i < _Batches.size(); i++)
		{
			auto obj = _Batches[i].get();
			if (obj->texture)
				if (obj->texture == texture)
					return obj;
		}
	}

	struct Line
	{
		float x1;
		float v1;
		float x2;
		float v2;
		SDL_Color color;
	};
	static std::vector<SDL_Vertex> _Vertices;
	static std::vector<int> _Indices;
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

	static Rendering::Batch* CreateBatch(SDL_Texture* texture);
public:
	static void Init(SDL_Window* window)
	{
		if (!_Renderer)
		{
			_Lines.reserve(10000);
			calculateTables();
			_Renderer = SDL_CreateRenderer(window, NULL);
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
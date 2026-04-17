#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <memory>
#include "../EntitySystem/Entities.h"

class Scene;

struct TilePositions
{
	int x, y;
};

class Rendering
{
public:
	class Debugger
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
		static bool DrawSpatialPartioning;
		static bool DrawTileMapColliders;

		static void RenderSpatialPartioning();
		static void RenderTileMapColliders();
		static void DrawCollisionBox(const Components::Collider2D::CollisionBox& box, const SDL_Color& color, bool solid);
		static std::vector<Box>& GetCollisionBoxes()
		{
			return collisionBoxes;
		}
	};

private:
	struct Batch
	{
		static int batchNumber;
		std::vector<SDL_Vertex> _Vertices = {};
		std::vector<int> _Indices = {};
		SDL_Texture* texture = nullptr;
		Scene* scene = nullptr;
		Batch()
		{
			_Indices.reserve(2000);
			_Vertices.reserve(1000);
			++batchNumber;
		}
	};

	static Batch m_ShadowBatch;
	static std::vector<std::unique_ptr<Batch>> m_Batches;
	static Rendering::Batch* _FindBatch(SDL_Texture* texture);
	static Rendering::Batch* _CreateBatch(SDL_Texture* texture);
	static Rendering::Batch* _FindOrCreateBatch(Components::Sprite * sprite);

	// Rendering layers, light layer, sprite layer 
	struct RenderingLayer
	{
		SDL_Texture* texture = nullptr;
		~RenderingLayer()
		{
			if (texture)
				SDL_DestroyTexture(texture);
		}
	};
	static RenderingLayer m_lightLayer;
	static RenderingLayer m_spriteLayer;

	//
	static SDL_Texture* lightTexture;
	static unsigned int lightUpdateInterval;
	static void RenderLights(std::vector<Object*>* objects, bool& lightsInScene);

	//Figure out better way to draw lines? Batching?
	struct Line
	{
		float x1;
		float v1;
		float x2;
		float v2;
		SDL_Color color;
	};
	static std::vector<Line> m_Lines;

	static SDL_Renderer* m_Renderer;
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
		if (!m_Renderer)
		{
			m_Lines.reserve(500);
			m_Batches.reserve(500);
			calculateTables();
			m_Renderer = SDL_CreateRenderer(window, NULL);
			SDL_SetRenderVSync(m_Renderer, SDL_RENDERER_VSYNC_DISABLED);
			SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND);
		}
	}
	static inline SDL_Renderer* GetRenderer()
	{
		return m_Renderer;
	}
	static void ClearScreen(SDL_Color color = { 0, 0, 0, 0 });
	static void DrawQuad(float x, float y, float w, float h, float rotation, Components::Sprite* sprite, const SDL_Color& color);
	static void DrawLine(float x1, float v1, float x2, float v2, SDL_Color color);
	static void PushToScreen();
	static void SetLightUpdateInterval(unsigned int val);
};
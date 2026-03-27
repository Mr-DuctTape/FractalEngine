#pragma once
#include <SDL3/SDL.h>
#include <vector>

class Scene;

class Rendering
{
private:
	struct Line
	{
		float x1;
		float y1;
		float x2;
		float y2;
		SDL_Color color;
	};
	static std::vector<int> Indices;
	static std::vector<SDL_Vertex> Vertices;
	static std::vector<Line> Lines;

	static SDL_Renderer* renderer;

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
		if (!renderer)
		{
			Vertices.reserve(10000);
			Indices.reserve(15000);
			Lines.reserve(10000);
			calculateTables();
			renderer = SDL_CreateRenderer(window, NULL);
		}
	}
	static SDL_Renderer* getRenderer()
	{
		return renderer;
	}
	static void clearScreen(SDL_Color color = { 0, 0, 0, 0 });
	static void drawQuad(float x, float y, float w, float h, float rotation, const SDL_Color& color);
	static void drawLine(float x1, float x2, float y1, float y2, SDL_Color color);
	static void pushToScreen();
};
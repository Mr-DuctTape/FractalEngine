#pragma once
#include "../EntitySystem/Entities.h"
#include "../Vectors/Vector2D.h"
#include "../Rendering/RenderingSystem.h"

namespace Physics
{
	class SpatialGrid
	{
	public:
		struct Cell
		{
			std::vector<GameObject*> objects;
		};
		static int cellSize;
		static int width, height;
	private:
		static std::vector<std::vector<Cell>> cells;
	public:
		inline static void CreateCells()
		{
			cells.resize(height);
			for (size_t y = 0; y < height; y++)
			{
				cells[y].resize(width);
			}
		}
		inline static Cell* WorldToCell(float x, float y)
		{
			int posX = (int)x / cellSize;
			int posY = (int)y / cellSize;

			if (posY < 0 || posX < 0 || posY >= cells.size() || posX >= cells[posY].size())
				return nullptr;

			return &cells[posY][posX]; 
		}
		inline static Cell* GetCell(int x, int y)
		{
			if (x < 0 || y < 0 || y >= cells.size() || x >= cells[y].size())
				return nullptr;

			return &cells[y][x];
		}
		inline static void InsertObject(GameObject* object)
		{
			int posX = (int)object->transform.position.x / cellSize;
			int posY = (int)object->transform.position.y / cellSize;

			if (posY < 0 || posX < 0 ||posY >= cells.size() || posX >= cells[posY].size())
				return;

			cells[posY][posX].objects.push_back(object);
		}
		inline static void RenderCell(int x, int y, const SDL_Color& color, bool solid)
		{
			Components::Collider2D::CollisionBox box;

			int posX = x * cellSize;
			int posY = y * cellSize;

			box.minX = posX;
			box.maxX = posX + cellSize;
			box.minY = posY;
			box.maxY = posY + cellSize;
			Rendering::Debugger::DrawCollisionBox(box, color, false);
		}
		inline static void RenderCells()
		{
			for (size_t y = 0; y < height; y ++)
			{
				for (size_t x = 0; x < width; x ++)
				{
					Components::Collider2D::CollisionBox box;

					int posX = x * cellSize;
					int posY = y * cellSize;

					box.minX = posX;
					box.maxX = posX + cellSize;
					box.minY = posY;
					box.maxY = posY + cellSize;
					Rendering::Debugger::DrawCollisionBox(box, {255, 155, 155, 255}, false);
				}
			}
		}
		inline static void Clear()
		{
			for (auto& column : cells)
			{
				for (auto& cell : column)
				{
					cell.objects.clear();
				}
			}
		}
	};

	namespace Functions
	{
		bool CheckCollision(const Components::Collider2D::CollisionBox& a, const Components::Collider2D::CollisionBox& b);
		inline void Collide(GameObject* obj, Object* obj2);
		inline void Gravity(Components::Physics2D* phys);
		inline void Movement(GameObject* obj, Components::Physics2D* physComp);
		inline void UpdatePhysics(GameObject* object);
	}

	namespace Values
	{
		const Vector2 worldGravity = { 0.0f, 1000.0f };
		constexpr float friction = 0.65f;
	}
	void Run(std::vector<Object*>& objects);
}
#pragma once
#include "../EntitySystem/Entities.h"
#include "../Vectors/Vector2D.h"

namespace Physics
{
	namespace Functions
	{
		bool CheckCollision(const Components::Collider2D::CollisionBox& a, const Components::Collider2D::CollisionBox& b);
		void Collide(GameObject* obj, Object* obj2);
		void Gravity(Components::Physics2D* phys);
		void Movement(GameObject* obj, Components::Physics2D* physComp);
		void UpdatePhysics(GameObject* object);
	}

	namespace Values
	{
		const Vector2 worldGravity = { 0.0f, 1000.0f };
		constexpr float friction = 0.65f;
	}
	void Run(std::vector<Object*>& objects);
}
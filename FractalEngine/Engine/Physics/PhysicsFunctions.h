#pragma once
#include "../EntitySystem/Entities.h"
#include "../Vectors/Vector2D.h"

namespace Physics
{
	struct CollisionInfo
	{
		bool collided = false;
		Vector2 direction = { 0,0 };
		float penetration = 0.0f;
	};

	namespace Functions
	{
		bool CheckCollision(const Components::CollisionBox& collBox1, const Components::CollisionBox& collBox2);
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
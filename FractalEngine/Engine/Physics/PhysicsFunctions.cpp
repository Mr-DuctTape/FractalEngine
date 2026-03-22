#include "PhysicsFunctions.h"
#include "../EntitySystem/Entities.h"
#include <iostream>

using namespace Components;

namespace Physics
{
	inline bool CheckCollision(const CollisionBox collBox1, const CollisionBox collBox2) // AABB Collision?
	{
		if (collBox1.minX <= collBox2.maxX && collBox1.maxX >= collBox2.minX &&
			collBox1.minY <= collBox2.maxY && collBox1.maxY >= collBox2.minY)
		{
			return true;
		}
		return false;
	}

	void Collide(GameObject* obj, GameObject* other)
	{
		Vector2 normal = (obj->transform.position - other->transform.position).normalized();

		Vector2 relativeVelocity = obj->transform.velocity - other->transform.velocity;
		float velAlongNormal = Vector2::Dot(relativeVelocity, normal);

		if (velAlongNormal > 0)
			return;


		Vector2 v = obj->transform.velocity;
		Vector2 n = normal;

		Vector2 reflected = v - 2.0f * Vector2::Dot(v, n) * n;

		obj->transform.velocity = reflected * 0.8f;
		other->transform.velocity = -reflected * 0.8f;

		Vector2 diff = obj->transform.position - other->transform.position;
		float distance = sqrt(diff.x * diff.x + diff.y * diff.y);

		float minDist = 1.0f; // depends on your object size
		float penetration = minDist - distance;

		if (penetration > 0)
		{
			Vector2 correction = normal * (penetration / 2.0f);
			obj->transform.position += correction;
			other->transform.position -= correction;
			return;
		}

		obj->transform.position += obj->transform.velocity;
		if(other->hasComponent<Physics2D>())
			other->transform.position += other->transform.velocity;
		std::cout << "X: " << obj->transform.velocity.x << " Y: " << obj->transform.velocity.y << "\n";
	}

	void Gravity(GameObject* obj, Physics2D* physicsComponent)
	{
		physicsComponent->currentFall = SDL_GetTicks();
		float time = (physicsComponent->currentFall - physicsComponent->lastFall) / 1000.0;
		obj->transform.velocity.y += Physics::Values::worldGravity * time;
	}

	void UpdatePhysics(GameObject* object)
	{
		Physics2D* physicsComponent = object->getComponent<Physics2D>();
		if (!physicsComponent)
			return;
		
		if (physicsComponent->gravity)
		{
			Gravity(object, physicsComponent);
			object->transform.position += object->transform.velocity;
		}

		bool anyCollision = false;
		for (auto& p : objects)
		{
			if (p.get() == object)
				continue;

			if (CheckCollision(object->getCollisionBox(), p->getCollisionBox()))
			{
				Collide(object, p.get());
				anyCollision = true;
			}
		}

		if (!anyCollision)
		{
			physicsComponent->lastFall = physicsComponent->currentFall;
		}
	}

	void Run() // Physics simulation on all GameObjects with Physics2D component
	{
		for (size_t i = 0; i < objects.size(); i++)
		{
			UpdatePhysics(objects[i].get());
		}
	}
}
#include "PhysicsFunctions.h"
#include "../EntitySystem/Entities.h"
#include <iostream>

namespace Physics
{
	static inline bool checkCollision(const Components::CollisionBox collBox1, const Components::CollisionBox collBox2) // AABB Collision?
	{
		if (collBox1.minX <= collBox2.maxX && collBox1.maxX >= collBox2.minX &&
			collBox1.minY <= collBox2.maxY && collBox1.maxY >= collBox2.minY)
		{
			std::cout << "Collision\n";
			return true;
		}
		std::cout << "No Collision\n";
		return false;
	}
	static void Collide(GameObject* obj, GameObject* other)
	{
		Components::Physics2D* physicsComponent = obj->getComponent<Components::Physics2D>();
		Components::Physics2D* otherPhysicsComponent = other->getComponent<Components::Physics2D>();

		float objMass = physicsComponent->mass;
		float otherMass = otherPhysicsComponent->mass;

		Vector2 objVel = obj->transform.velocity;
		Vector2 otherVel = obj->transform.velocity;

		//Todo: Figure out how collisions work
	}
	static inline void updatePhysics(GameObject* object)
	{
		Components::CollisionBox collBox = object->getCollisionBox();
		for (size_t i = 0; i < objects.size(); i++)
		{
			auto b = objects[i].get();
			if (b == object)
				continue;

			Components::CollisionBox br = b->getCollisionBox();
			if (checkCollision(collBox, br))
			{
				//Collision happens
				Collide(object, b);
			}
		}
	}
	void Run() // Physics simulation on all GameObjects with Physics2D component
	{
		GameObject* obj = nullptr;
		for (size_t i = 0; i < objects.size(); i++)
		{
			obj = objects[i].get();
			if(obj->getComponent<Components::Physics2D>())
				updatePhysics(obj);
		}
	}
}
#include "PhysicsFunctions.h"
#include "../EntitySystem/Entities.h"

namespace Physics
{
	static inline bool checkCollision(CollisionBox collBox1, CollisionBox collBox2) // AABB Collision?
	{
		if (collBox1.minX <= collBox2.maxX && collBox1.maxX >= collBox2.minX && 
			collBox1.minY <= collBox2.maxY && collBox1.maxY >= collBox2.minY)
			return true;

		return false;
	}
	static void Collide(GameObject* obj, GameObject* other)
	{
		float objMass = obj->physics2D->mass;
		float otherMass = other->physics2D->mass;

		Vector2 objVel = obj->transform.velocity;
		Vector2 otherVel = obj->transform.velocity;

		//Figure out how collisions work, cause I have no fucking clue rn
	}
	static inline void updatePhysics(GameObject* object)
	{
		CollisionBox collBox = object->getCollisionBox();
		for (size_t i = 0; i < objects.size(); i++)
		{
			auto b = objects[i].get();
			if (checkCollision(collBox, b->getCollisionBox()))
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
			if(obj->physics2D)
				updatePhysics(obj);

			obj->transform.position += obj->transform.velocity;
		}
	}
}
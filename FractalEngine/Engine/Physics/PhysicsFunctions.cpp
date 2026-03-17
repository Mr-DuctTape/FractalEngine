#include "PhysicsFunctions.h"
#include "../EntitySystem/Entities.h"
#include <iostream>

using namespace Components;

namespace Physics
{
	static inline bool checkCollision(const CollisionBox collBox1, const CollisionBox collBox2) // AABB Collision?
	{
		if (collBox1.minX <= collBox2.maxX && collBox1.maxX >= collBox2.minX &&
			collBox1.minY <= collBox2.maxY && collBox1.maxY >= collBox2.minY)
		{
			return true;
		}
		return false;
	}

	static void Collide(GameObject* obj, GameObject* other)
	{
		//Todo: Figure out how collisions work
		Vector2 velocity = obj->transform.velocity;
		Vector2 otherVelocity = other->transform.velocity;

		Vector2 temp = velocity;

		velocity = otherVelocity * Values::friction;
		otherVelocity = velocity * Values::friction;

		obj->transform.position += velocity;
		other->transform.position += otherVelocity;
	}

	static inline void updatePhysics(GameObject* object)
	{
		if (!object->hasComponent<Physics2D>())
			return;

		int* beginFall = &object->getComponent<Physics2D>()->currentFall;
		int* lastFall = &object->getComponent<Physics2D>()->lastFall;

		CollisionBox currentBox = object->getCollisionBox();
		GameObject* collisionObj = nullptr;
		bool isColliding = false;

		for (size_t i = 0; i < objects.size(); i++)
		{
			collisionObj = objects[i].get();
			if (collisionObj == object)
				continue;

			CollisionBox otherBox = collisionObj->getCollisionBox();
			isColliding = checkCollision(currentBox, otherBox);
			if (isColliding)
				break;
		}

		if (!isColliding)
		{
			*beginFall = SDL_GetTicks();
			float time = (*beginFall - *lastFall) / 1000.0;

			//Apply gravity
			Transform* objTransform = object->getComponent<Transform>();
			objTransform->velocity.y = Values::worldGravity * time;
			objTransform->position += objTransform->velocity;
		}
		else
		{
			*lastFall = SDL_GetTicks();
			Transform* objTransform = &object->transform;
			Sprite* objSprite = collisionObj->getComponent<Sprite>();

			Collide(object, collisionObj);
		}
	}

	void Run() // Physics simulation on all GameObjects with Physics2D component
	{
		GameObject* obj = nullptr;
		for (size_t i = 0; i < objects.size(); i++)
		{
			obj = objects[i].get();
			updatePhysics(obj);
		}
	}
}
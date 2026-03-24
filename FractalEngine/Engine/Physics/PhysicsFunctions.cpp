#include "PhysicsFunctions.h"
#include "../EntitySystem/Entities.h"
#include "../Core/FractalEngineCore.h"

using namespace Components;

namespace Functions
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
		Physics2D* physicsComponent = obj->getComponent<Physics2D>();

		if (!physicsComponent) return;

		CollisionBox objBox = obj->getCollisionBox();
		CollisionBox otherBox = other->getCollisionBox();

		float penetrationX = std::min(objBox.maxX, otherBox.maxX) - std::max(objBox.minX, otherBox.minX);
		float penetrationY = std::min(objBox.maxY, otherBox.maxY) - std::max(objBox.minY, otherBox.minY);
		if (penetrationX < penetrationY)
		{
			if (obj->transform.position.x < other->transform.position.x)
				obj->transform.position.x -= penetrationX;
			else
				obj->transform.position.x += penetrationX;
		}
		else
		{
			if (obj->transform.position.y < other->transform.position.y)
				obj->transform.position.y -= penetrationY;
			else
				obj->transform.position.y += penetrationY;
		}

		Vector2 normal = (obj->transform.position - other->transform.position).normalized();
		if (normal.length() == 0) normal = Vector2(0, 1);
		else normal = normal.normalized();

		Vector2 direction = (physicsComponent->velocity).normalized();
		Vector2 newDir = (normal - direction).normalized();

		Physics2D* otherPhysics = other->getComponent<Physics2D>();;
		if (otherPhysics)
		{
			otherPhysics->velocity = -Vector2::reflect(physicsComponent->velocity, normal) * Physics::Values::friction;
		}
		physicsComponent->velocity = Vector2::reflect(physicsComponent->velocity, normal) * Physics::Values::friction;
	}

	void UpdatePhysics(GameObject* object)
	{
		Physics2D* physicsComponent = object->getComponent<Physics2D>();
		if (!physicsComponent)
			return;

		physicsComponent->acceleration = physicsComponent->force / physicsComponent->mass;

		bool anyCollision = false;
		auto &objects = SceneManager::getCurrentScene()->objects;

		for (auto& p : objects)
		{
			auto other = dynamic_cast<GameObject*>(p);

			if (!other) 
				continue;
			if (other == object)
				continue;

			if (CheckCollision(object->getCollisionBox(), other->getCollisionBox()))
			{
				Collide(object, other);
				anyCollision = true;
			}
		}
		if (!anyCollision)
		{
			if (physicsComponent->useGravity)
			{
				physicsComponent->acceleration.y += Physics::Values::worldGravity;
			}
		}

		float& time = FractalEngineCore::deltaTime;
		physicsComponent->velocity += physicsComponent->acceleration * time;
		object->transform.position += physicsComponent->velocity * time;

		physicsComponent->force.x = 0;
		physicsComponent->force.y = 0;
	}
}

void Physics::Run(std::vector<Object*>& objects) // Physics simulation on all GameObjects with Physics2D component
{
	for (size_t i = 0; i < objects.size(); i++)
	{
		GameObject* obj = dynamic_cast<GameObject*>(objects[i]);
		if(obj)
			Functions::UpdatePhysics(obj);
	}
}
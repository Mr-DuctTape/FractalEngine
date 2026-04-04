#include "PhysicsFunctions.h"
#include "../EntitySystem/Entities.h"
#include "../Core/FractalEngineCore.h"

using namespace Components;

inline bool Physics::Functions::CheckCollision(const CollisionBox& a, const CollisionBox& b) // AABB Collision?
{
	if (a.minX <= b.maxX && a.maxX >= b.minX && a.minY <= b.maxY && a.maxY >= b.minY)
	{
		return true;
	}
	return false;
}

void Physics::Functions::Collide(GameObject* obj, GameObject* other, const Physics::CollisionInfo& info)
{
	if (!obj || !other) return;
}

inline void Physics::Functions::Gravity(Physics2D* phys)
{
	if (!phys) return;
	const Vector2 worldGravity = { 0, 1000.0f };
	phys->Accelerate(worldGravity);
}

inline void Physics::Functions::Movement(GameObject* obj, Components::Physics2D* physComp)
{
	if (!physComp) return;

	const Vector2 velocity = physComp->position_current - physComp->position_old;
	// Save current position
	physComp->position_old = physComp->position_current;
	// Perform verlet intergration

	physComp->position_current = physComp->position_current + velocity + physComp->acceleration * FractalEngineCore::deltaTime * FractalEngineCore::deltaTime;

	// Reset acceleration and set position
	obj->transform.position = physComp->position_current;
	physComp->acceleration = { 0,0 };
}

void Physics::Functions::UpdatePhysics(GameObject* object)
{
	Physics2D* physicsComponent = object->GetComponent<Physics2D>();
	auto& objectList = SceneManager::GetCurrentScene()->objects;
	Gravity(physicsComponent);
	Movement(object, physicsComponent);

	for (size_t j = 0; j < objectList.size(); j++)
	{
		if (objectList[j]->GetType() != GAMEOBJECT) continue;
		GameObject* other = static_cast<GameObject*>(objectList[j]);
		if (other == object) continue;

		Physics2D* otherComponent = other->GetComponent<Physics2D>();
		const Vector2 objectVec = (physicsComponent) ? physicsComponent->position_current : object->transform.position;
		const Vector2 otherVec = (otherComponent) ? otherComponent->position_current : other->transform.position;
		const Vector2 collision_axis = objectVec - otherVec;

		if (CheckCollision(object->GetCollisionBox(), other->GetCollisionBox()))
		{
			auto A = object->GetCollisionBox();
			auto B = other->GetCollisionBox();

			float overlapX = std::min(A.maxX, B.maxX) - std::max(A.minX, B.minX);
			float overlapY = std::min(A.maxY, B.maxY) - std::max(A.minY, B.minY);

			if (overlapX <= 0 || overlapY <= 0)
				continue;

			if (overlapX < overlapY)
			{
				float delta = (collision_axis.x < 0) ? -1.0f : 1.0f;
				//Fix collision on X
				if (physicsComponent)
				{
					physicsComponent->position_current.x += overlapX * 0.9f * delta;
					physicsComponent->position_old.x = physicsComponent->position_current.x;
				}
			}
			else
			{
				float delta = (collision_axis.y < 0) ? -1.0f : 1.0f;
				//Fix collision on Y
				if (physicsComponent)
				{
					physicsComponent->position_current.y += overlapY * delta;
					physicsComponent->position_old.y = physicsComponent->position_current.y;
				}
			}
		}
	}
}


void Physics::Run(std::vector<Object*>& objects) // Physics simulation on all GameObjects with Physics2D component
{
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i]->GetType() != Type::GAMEOBJECT) continue;
		GameObject* obj = static_cast<GameObject*>(objects[i]);
		Functions::UpdatePhysics(obj);
	}
}
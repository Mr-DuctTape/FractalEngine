#include "PhysicsFunctions.h"
#include "../EntitySystem/Entities.h"
#include "../SceneManagement/FractalScene.h"
#include "../Core/FractalEngineCore.h"
#include "../Rendering/RenderingSystem.h"

using namespace Components;

inline bool Physics::Functions::CheckCollision(const Components::Collider2D::CollisionBox& a, const Components::Collider2D::CollisionBox& b) // AABB Collision?
{
	if (a.minX <= b.maxX && a.maxX >= b.minX && a.minY <= b.maxY && a.maxY >= b.minY)
	{
		return true;
	}
	return false;
}

struct TilePosition
{
	unsigned int x;
	unsigned int y;
};
void Physics::Functions::Collide(GameObject* obj, Object* obj2)
{
	if (!obj || !obj2) return;
	Physics2D* physicsComponent = obj->GetComponent<Physics2D>();
	if(physicsComponent)
		physicsComponent->isGrounded = false;

	if (obj2->GetType() == TILEMAP)
	{
		TileMap* tileMap = static_cast<TileMap*>(obj2);
		if (!physicsComponent)
			return;

		//Convert world coordinates -> tile coordinates
		auto scale = tileMap->GetTileScale();
		unsigned int objX = (unsigned int)(obj->transform.position.x / scale.combinedX);
		unsigned int objY = (unsigned int)(obj->transform.position.y / scale.combinedY);

		//Only check tiles close too the object to save performance
		TilePosition positions[9] =
		{
			{objX, objY}, //Check the obj positon so it doesnt phase through stuff
			{ objX - 1, objY }, { objX + 1, objY },
			{ objX, objY - 1 }, { objX, objY + 1 },
			{ objX + 1, objY + 1 }, { objX - 1, objY + 1 },
			{ objX + 1, objY - 1 },  {objX - 1, objY - 1 }
		};
		unsigned int size = sizeof(positions) / sizeof(positions[0]);

		for (size_t i = 0; i < size; i++)
		{
			unsigned int x = positions[i].x;
			unsigned int y = positions[i].y;

			auto objectCollisionBox = obj->GetCollisionBox();
			auto tileCollisionBox = tileMap->GetTileCollisionBox(x, y);
			bool tileCollidable = (tileMap->IsTileSolid(x, y) && tileMap->CanTileCollideWith(x, y, obj->GetComponent<Collider2D>()));

			//Tile debug
			if(tileMap->debugMode == TileMap::TileDebugMode::NEARBY && !tileCollidable)
				Rendering::Debug::DrawCollisionBox(tileCollisionBox, { 255, 255, 255, 255}, false);
			if (tileMap->debugMode == TileMap::TileDebugMode::NEARBY && tileCollidable)
				Rendering::Debug::DrawCollisionBox(tileCollisionBox, { 255, 0, 0, 96 }, true);

			if (tileCollidable && CheckCollision(objectCollisionBox, tileCollisionBox))
			{
				if(tileMap->debugMode)
					Rendering::Debug::DrawCollisionBox(tileCollisionBox, { 40, 255, 40, 128 }, true);

				Vector2 objectVec = physicsComponent->position_current;
				Vector2 otherVec = { (tileCollisionBox.minX + tileCollisionBox.maxX) * 0.5f, (tileCollisionBox.minY + tileCollisionBox.maxY) * 0.5f}; // Center of tile
				Vector2 collision_axis = physicsComponent->position_old - otherVec;

				float overlapX = std::min(objectCollisionBox.maxX, tileCollisionBox.maxX) - std::max(objectCollisionBox.minX, tileCollisionBox.minX);
				float overlapY = std::min(objectCollisionBox.maxY, tileCollisionBox.maxY) - std::max(objectCollisionBox.minY, tileCollisionBox.minY);

				if (physicsComponent->isGrounded && overlapY < 0.01f)
				{
					continue; // ignore this collision
				}
				if (overlapX < overlapY)
				{
					float direction = (collision_axis.x < 0) ? -1.0f : 1.0f;
					//Fix collision on X
					if (physicsComponent)
					{
						physicsComponent->position_current.x += overlapX * direction;
						physicsComponent->position_old.x = physicsComponent->position_current.x;
					}
				}
				else
				{
					float direction = (collision_axis.y < 0) ? -1.0f : 1.0f;
					//Fix collision on Y
					if (physicsComponent)
					{
						physicsComponent->position_current.y += overlapY * direction;
						physicsComponent->position_old.y = physicsComponent->position_current.y;
					}
				}
			}
		}
	}
	else if (obj2->GetType() == GAMEOBJECT)
	{
		GameObject* other = static_cast<GameObject*>(obj2);
		if (!obj->HasComponent<Collider2D>() || !other->HasComponent<Collider2D>())
			return;

		Physics2D* otherComponent = other->GetComponent<Physics2D>();
		Vector2 objectVec = (physicsComponent) ? physicsComponent->position_current : obj->transform.position;
		Vector2 otherVec = (otherComponent) ? otherComponent->position_current : other->transform.position;
		Vector2 collision_axis = objectVec - otherVec;

		if (CheckCollision(obj->GetCollisionBox(), other->GetCollisionBox()))
		{
			auto A = obj->GetCollisionBox();
			auto B = other->GetCollisionBox();

			float overlapX = std::min(A.maxX, B.maxX) - std::max(A.minX, B.minX);
			float overlapY = std::min(A.maxY, B.maxY) - std::max(A.minY, B.minY);

			if (overlapX < overlapY)
			{
				//Fix collision on X
				float direction = (collision_axis.x < 0) ? -1.0f : 1.0f;
				if (physicsComponent)
				{
					physicsComponent->position_current.x += overlapX * 0.9f * direction;
					physicsComponent->position_old.x = physicsComponent->position_current.x;
				}
			}
			else
			{
				//Fix collision Y
				float direction = (collision_axis.y < 0) ? -1.0f : 1.0f;
				if (physicsComponent)
				{
					physicsComponent->position_current.y += overlapY * direction;
					physicsComponent->position_old.y = physicsComponent->position_current.y;
				}
			}
		}
	}
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
		if (objectList[j]->GetType() != GAMEOBJECT && objectList[j]->GetType() != TILEMAP)
			continue;
		if (objectList[j]->GetType() == GAMEOBJECT)
		{
			GameObject* other = static_cast<GameObject*>(objectList[j]);
			if (other == object)
				continue;
		}
		Collide(object, objectList[j]);
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
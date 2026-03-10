#include "PhysicsFunctions.h"
namespace Physics
{
	void updatePhysics(GameObject* object)
	{
		float mass = object->physics2D->mass;
		float gravity = object->physics2D->gravity;
		float friction = object->physics2D->friction;

		Vector2 velocity = object->transform.velocity;
		Vector2 positiin = object->transform.position;
	}
}
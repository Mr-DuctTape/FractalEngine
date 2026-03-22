#include "Entities.h"

unsigned int GameObject::IDNumber = 0;

std::vector<std::unique_ptr<GameObject>> objects = {};

void Components::Physics2D::addForce(const Vector2 force)
{
	beginForceTime = SDL_GetTicks();
	physicsForce = force;
}

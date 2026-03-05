#include "Entities.h"
#include "../Vectors/Vector2D.h"

#include <SDL3/SDL.h>

std::vector<std::unique_ptr<GameObject>> Objects = {};

class GameObject
{
public:
	SDL_Texture* texture;
	SDL_FColor color;
	Vector2 position;
	Vector2 velocity;
	float width, height;

	GameObject(SDL_Texture* t, SDL_FColor c, Vector2 pos, Vector2 vel, float w, float h) :
		texture(t), color(c), position(pos), velocity(vel), width(w), height(h)
	{
		Objects.push_back(std::make_unique<GameObject>(this));
	}
	~GameObject()
	{
		for (auto it = Objects.begin(); it != Objects.end(); it++)
		{
			if (it->get() == this)
			{
				Objects.erase(it);
				break;
			}
		}
	}
};
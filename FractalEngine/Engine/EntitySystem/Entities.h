#pragma once
#include <vector>
#include <memory>
#include <SDL3/SDL.h>
#include "../Vectors/Vector2D.h"

//Components

struct Transform
{
	Vector2 velocity;
	Vector2 position;
	float rotation = 0.0;
};

struct Sprite
{
	SDL_Texture* texture;
	SDL_Color color;
	float width, height;
};

struct Physics2D
{
	float gravity = 1.0f;
	float mass = 1.0f;
	float friction = 0.2f;
};

///

class Camera;

class GameObject
{
public:
	Transform transform = {};
	Sprite* sprite = nullptr;
	Physics2D* physics2D = nullptr;

	~GameObject()
	{
		if (sprite)
			delete sprite;
		if (physics2D)
			delete physics2D;
	}
};

extern std::vector<std::unique_ptr<GameObject>> objects;

template <typename T>
T& CreateObject()
{
	auto obj = std::make_unique<T>();
	auto location = obj.get();
	objects.push_back(std::move(obj));
	return *location;
}

template<typename T>
T* addComponent(T* value)
{
	if (value)
	{
		return new T(*value);
	}

	return new T;
}
#pragma once
#include <vector>
#include <memory>
#include <SDL3/SDL.h>
#include "../Vectors/Vector2D.h"

//
namespace Components
{
	struct Component
	{
		virtual ~Component() {};
	};

	struct Transform : public Component
	{
		Vector2 velocity;
		Vector2 position;
		float rotation = 0.0;
	};

	struct Sprite : public Component
	{
		SDL_Texture* texture;
		SDL_Color color;
		float width, height;
	};

	struct Physics2D : public Component
	{
		float gravity = 1.0f;
		float mass = 1.0f;
		float friction = 0.2f;
		float bouncyness = 0.1f;
	};

	struct CollisionBox : public Component
	{
		CollisionBox()
		{
			minY = 0;
			maxY = 0;
			minX = 0;
			maxX = 0;
		}
		CollisionBox(float mnY, float mxY, float mnX, float mxX)
		{
			minY = mnY;
			maxY = mxY;
			minX = mnX;
			maxX = mxX;
		}
		float minY;
		float maxY;
		float minX;
		float maxX;
	};
}
//

class Camera
{
public:
	Components::Transform transform = {};
};

class GameObject
{
private:
	std::vector<Components::Component*> objComponents;
public:
	Components::Transform transform = {};

	inline Components::CollisionBox getCollisionBox()
	{
		Components::Sprite* sprt = getComponent<Components::Sprite>();

		if (sprt != nullptr)
		{
			return Components::CollisionBox{
				transform.position.y,                 // minY
				transform.position.y + sprt->height,  // maxY
				transform.position.x,                 // minX
				transform.position.x + sprt->width    // maxX
			};
		}
		else
		{
			return Components::CollisionBox{
				transform.position.y,
				transform.position.y + 150,
				transform.position.x,
				transform.position.x + 150
			};
		}

	}

	template <typename T>
	void addComponent()
	{
		static_assert(std::is_base_of_v<Components::Component, T>,
			"T must be a component!");

		for (size_t i = 0; i < objComponents.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(objComponents[i]))
			{
				return;
			}
		}
		T* location = new T();
		objComponents.push_back(location);
	}
	template <typename T>
	bool hasComponent() const
	{
		static_assert(std::is_base_of_v<Components::Component, T>,
			"T must be a component!");

		for (size_t i = 0; i < objComponents.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(objComponents[i]))
			{
				return true;
			}
		}
		return false;
	}
	template <typename T>
	T* getComponent()
	{
		static_assert(std::is_base_of_v<Components::Component, T>,
			"T must be a component!");

		for (size_t i = 0; i < objComponents.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(objComponents[i]))
			{
				return component;
			}
		}
		return nullptr;
	}
	~GameObject()
	{
		for (size_t i = 0; i < objComponents.size(); i++)
		{
			delete objComponents[i];
		}
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
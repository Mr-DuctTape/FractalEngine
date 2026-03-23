#pragma once
#include <vector>
#include <memory>
#include <SDL3/SDL.h>
#include "../Vectors/Vector2D.h"

class GameObject;

extern unsigned int* screenWidth;
extern unsigned int* screenHeight;

//
namespace Components
{
	struct Component
	{
		GameObject* parent = nullptr;
		virtual ~Component() {};
	};

	struct Transform : public Component
	{
		Vector2 position;
		float rotation = 0.0;
	};

	struct Sprite : public Component
	{
		SDL_Texture* texture;
		SDL_Color color;
		float width;
		float height;
	};

	struct Physics2D : public Component
	{
		Vector2 acceleration;
		Vector2 velocity;
		Vector2 force;

		float mass = 1.0f;
		bool useGravity = true;
		void addForce(const Vector2& force_)
		{
			force += (force_ * 1000.0);
		}
	};

	struct CollisionBox
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

class GameObject
{
private:
	std::vector<Components::Component*> objComponents;
	inline void push_back(Components::Component* component)
	{
		component->parent = this;
		objComponents.push_back(component);
	}
protected:
	static unsigned int IDNumber;
public:
	Components::Transform transform = {};
	unsigned int ID;

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
	int getComponentAmount() const
	{
		return objComponents.size();
	}
	template <typename T>
	void addComponent(const T& value)
	{
		static_assert(std::is_base_of_v<Components::Component, T>,"T must be a component!");
		for (size_t i = 0; i < objComponents.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(objComponents[i]))
			{
				return;
			}
		}
		T* location = new T(value);
		push_back(location);
	}
	template <typename T>
	void addComponent()
	{
		static_assert(std::is_base_of_v<Components::Component, T>,"T must be a component!");
		for (size_t i = 0; i < objComponents.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(objComponents[i]))
			{
				return;
			}
		}
		T* location = new T();
		push_back(location);
	}
	template <typename T>
	bool hasComponent() const
	{
		static_assert(std::is_base_of_v<Components::Component, T>,"T must be a component!");
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
		static_assert(std::is_base_of_v<Components::Component, T>,"T must be a component!");
		for (size_t i = 0; i < objComponents.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(objComponents[i]))
			{
				return component;
			}
		}
		return nullptr;
	}
	GameObject()
	{
		IDNumber++;
		ID = IDNumber;
		objComponents.push_back(&transform);
	}
	~GameObject()
	{
		for (size_t i = 0; i < objComponents.size(); i++)
		{
			delete objComponents[i];
		}
	}
};

class Camera
{
public:
	Components::Transform transform;
	void follow(const GameObject& other)
	{
		transform.position.x = other.transform.position.x - *screenWidth / 2;
		transform.position.y = other.transform.position.y - *screenHeight / 2;
	}
	void follow(const Vector2& position)
	{
		transform.position.x = position.x - *screenWidth / 2;
		transform.position.y = position.y - *screenHeight / 2;
	}
};

extern Camera camera;
extern std::vector<std::unique_ptr<GameObject>> objects;

template <typename T>
T& CreateObject()
{
	auto obj = std::make_unique<T>();
	auto location = obj.get();
	objects.push_back(std::move(obj));
	return *location;
}
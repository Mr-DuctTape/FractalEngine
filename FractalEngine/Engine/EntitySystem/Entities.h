#pragma once
#include <vector>
#include <memory>
#include <SDL3/SDL.h>
#include "../Vectors/Vector2D.h"
class SceneManager;
#include "../SceneManagement/FractalScene.h"

enum Type
{
	OBJECT,
	GAMEOBJECT,
	OTHER
};

class Object
{
public:
	virtual ~Object() {};
	virtual Type getType() { return Type::OBJECT; };
};

//
class Rendering;
class GameObject;

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

	class Animator : public Component
	{
	public:
		struct Animation
		{
			unsigned int numberOfFrames;
			unsigned int frameIndex;

			float xOffset = 0.0;
			float yOffset = 0.0;

			float timer;
			float frameTime;

			SDL_FRect frame;
			SDL_FRect* renderTarget;
			SDL_Texture* spriteSheet;
		};
	private:
		static std::unordered_map<std::string, Animation> animations;
		Animation* currentAnimation = nullptr;
		friend class Rendering;
		float animationSpeed = 0.0f;
	public:
		void createAnimation
		(
			const std::string& name,
			unsigned int frames,
			float speed,
			SDL_Texture* spriteSheet
		);
		void Update(const float deltaTime);
		bool isPlaying = false;
		void setAnimation(const std::string& name);
		std::vector<std::string> getAnimationNames()
		{
			std::vector<std::string> temp;
			for (auto& pair : animations)
				temp.push_back(pair.first);
			return temp;
		}
		Animation& getCurrentAnimation() const
		{
			return *currentAnimation;
		}
		void Play();
		void setSpeed(const float& speed);
		void Stop();
	};

	struct CollisionBox
	{
		float minY;
		float maxY;
		float minX;
		float maxX;
		CollisionBox()
			: minY(0), maxY(0), minX(0), maxX(0)
		{ }
		CollisionBox(float mnY, float mxY, float mnX, float mxX)
			: minY(mnY), maxY(mxY), minX(mnX), maxX(mxX)
		{ }
	};
}

class TileMap
{

};

class GameObject : public Object
{
private:
	std::vector<Components::Component*> objComponents;
	inline void push_back(Components::Component* component)
	{
		component->parent = this;
		objComponents.push_back(component);
	}
	static unsigned int IDNumber;
	SDL_FRect rect;
public:
	Components::Transform transform = {};
	unsigned int ID;

	inline SDL_FRect& getRenderTarget()
	{
		if(rect.x != transform.position.x)
			rect.x = transform.position.x;
		if(rect.y != transform.position.y)
			rect.y = transform.position.y;
		return rect;
	}
	Type getType() override
	{
		return Type::GAMEOBJECT;
	};
	inline Components::CollisionBox getCollisionBox()
	{
		return Components::CollisionBox{
			transform.position.y,                 // minY
			transform.position.y + getRenderTarget().h,  // maxY
			transform.position.x,                 // minX
			transform.position.x + getRenderTarget().w    // maxX
		};
	}
	int getComponentAmount() const
	{
		return objComponents.size();
	}
	template <typename T>
	void addComponent(const T& value)
	{
		static_assert(std::is_base_of_v<Components::Component, T>, "T must be a component!");
		for (size_t i = 0; i < objComponents.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(objComponents[i]))
			{
				return;
			}
		}
		T* component = new T(value);
		push_back(component);
	}
	template <typename T>
	void addComponent()
	{
		static_assert(std::is_base_of_v<Components::Component, T>, "T must be a component!");
		for (size_t i = 0; i < objComponents.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(objComponents[i]))
			{
				return;
			}
		}
		T* component = new T();
		push_back(component);
	}
	template <typename T>
	bool hasComponent() const
	{
		static_assert(std::is_base_of_v<Components::Component, T>, "T must be a component!");
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
		static_assert(std::is_base_of_v<Components::Component, T>, "T must be a component!");
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
		rect.w = 150;
		rect.h = 150;

		ID = ++IDNumber;
		objComponents.push_back(&transform);
	}
	GameObject(unsigned int width, unsigned int height)
	{
		rect.w = width;
		rect.h = height;

		ID = ++IDNumber;
		objComponents.push_back(&transform);
	}
	~GameObject()
	{
		for (auto objComp : objComponents)
		{
			if (objComp == &transform) continue;
			delete objComp;
		}
	}
};

class Camera
{
private:
	static unsigned int& screenWidth;
	static unsigned int& screenHeight;
public:
	Components::Transform transform;
	void follow(const GameObject& other)
	{
		transform.position.x = other.transform.position.x - screenWidth / 2;
		transform.position.y = other.transform.position.y - screenHeight / 2;
	}
	void follow(const Vector2& position)
	{
		transform.position.x = position.x - screenWidth / 2;
		transform.position.y = position.y - screenHeight / 2;
	}
};

extern Camera camera;

template <typename T> //Creates object on the current active scene
T& CreateObject()
{
	static_assert(std::is_base_of_v<Object, T>, "T must be an Object!");
	auto location = new T();
	SceneManager::getCurrentScene()->objects.push_back(location);
	return *location;
}
template <typename T>
T& CreateObject(const T& value)
{
	static_assert(std::is_base_of_v<Object, T>, "T must be an Object!");
	auto location = new T(value);
	SceneManager::getCurrentScene()->objects.push_back(location);
	return *location;
}
#pragma once
#include <vector>
#include <memory>
#include <SDL3/SDL.h>
#include "../Vectors/Vector2D.h"
class SceneManager;
#include "../SceneManagement/FractalScene.h"

#include <iostream>

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
	virtual Type GetType() { return Type::OBJECT; };
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
		SDL_Texture* texture = nullptr;
		SDL_Color color = {255, 255, 255, 255};

		float u1 = 0.0f;
		float v1 = 0.0f;
		float u2 = 1.0f;
		float v2 = 1.0f;

		void ResetUV()
		{
			u1 = 0.0f;
			v1 = 0.0f;
			u2 = 1.0f;
			v2 = 1.0f;
		}
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
			unsigned int numberOfFrames = 0;
			unsigned int frameIndex = 0;

			float timer = 0.0;
			float frameTime = 0.0;

			unsigned int spriteY = 0;
			unsigned int spriteX = 0;

			unsigned int spriteWidth = 0;
			unsigned int spriteHeight = 0;

			SDL_Texture* spriteSheet = nullptr;
			SDL_Texture* savedSprite = nullptr;
		};
	private:
		static std::unordered_map<std::string, Animation> _animations;
		Animation _currentAnimation;
		Sprite* sprite = nullptr;
		float _animationSpeed = 0.0f;
		friend class Rendering;

		bool _hasInit = false;
		void Init();
	public:
		bool isPlaying = false;
		void CreateAnimation
		(
			const std::string& name,
			unsigned int frames,
			float speed,
			SDL_Texture* spriteSheet
		);
		std::vector<std::string> GetAnimationNames()
		{
			std::vector<std::string> temp;
			for (auto& pair : _animations)
				temp.push_back(pair.first);
			return temp;
		}
		Animation* GetCurrentAnimation() 
		{
			return &_currentAnimation;
		}
		void Update(const float deltaTime);
		void SetAnimation(const std::string& name);
		void SetSpeed(float speed);
		void Play();
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
		{
		}
		CollisionBox(float mnY, float mxY, float mnX, float mxX)
			: minY(mnY), maxY(mxY), minX(mnX), maxX(mxX)
		{
		}
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

	inline SDL_FRect& GetRenderTarget()
	{
		if (rect.x != transform.position.x)
			rect.x = transform.position.x;
		if (rect.y != transform.position.y)
			rect.y = transform.position.y;
		return rect;
	}
	Type GetType() override
	{
		return Type::GAMEOBJECT;
	};
	inline Components::CollisionBox GetCollisionBox()
	{
		return Components::CollisionBox{
			transform.position.y,                 // minY
			transform.position.y + GetRenderTarget().h,  // maxY
			transform.position.x,                 // minX
			transform.position.x + GetRenderTarget().w    // maxX
		};
	}
	size_t GetComponentAmount() const
	{
		return objComponents.size();
	}
	template <typename T>
	T* AddComponent(const T& value)
	{
		static_assert(std::is_base_of_v<Components::Component, T>, "T must be a component!");
		for (size_t i = 0; i < objComponents.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(objComponents[i]))
			{
				return component;
			}
		}
		T* component = new T(value);
		push_back(component);
		return component;
	}
	template <typename T>
	T* AddComponent()
	{
		static_assert(std::is_base_of_v<Components::Component, T>, "T must be a component!");
		for (size_t i = 0; i < objComponents.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(objComponents[i]))
			{
				return component;
			}
		}
		T* component = new T();
		push_back(component);
		return component;
	}
	template <typename T>
	bool HasComponeny() const
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
	T* GetComponent()
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
	SceneManager::GetCurrentScene()->objects.push_back(location);
	return *location;
}
template <typename T>
T& CreateObject(const T& value)
{
	static_assert(std::is_base_of_v<Object, T>, "T must be an Object!");
	auto location = new T(value);
	SceneManager::GetCurrentScene()->objects.push_back(location);
	return *location;
}
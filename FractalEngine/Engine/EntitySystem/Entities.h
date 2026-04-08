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
	TILEMAP,
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
		virtual void Init(const Vector2 pos) {};
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

		bool flippedX = false;
		bool flippedY = false;

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
		Vector2 position_current = { 0,0 };
		Vector2 position_old = { 0,0 };
		Vector2 acceleration = { 0,0 };

		float mass = 1.0f;
		float friction = 0.9f;
		bool useGravity = true;

		void Init(const Vector2 pos) override
		{
			position_current = pos;
			position_old = position_current;
		}

		void AddForce(const Vector2& force_)
		{
			position_current += force_;
		}
		void Accelerate(Vector2 acc)
		{
			acceleration += acc;
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

class TileMap : public Object
{
private:
	struct TileSet
	{
		SDL_Texture* spriteSheet = nullptr;
		unsigned int numberOfTiles = 0;
	};
	struct TileProperties
	{
		unsigned int ID = 0;
		bool isCollidable = false;
		float friction = 0.0f;
	};
	TileSet _currentTileSet = {};

	std::vector<TileProperties> _tileProperties;
	std::vector<std::vector<unsigned int>> _tiles;

	unsigned int _tilePixelWidth = 0, _tilePixelHeight = 0;
	float _tileScaleX = 0.0f, _tileScaleY = 0.0f;
	TileMap::TileProperties _GetTileProperties(unsigned int ID);

public:
	enum TileDebugMode
	{
		NONE,
		NEARBY,
		FULL
	};

	Vector2 position = {0.0f,0.0f};
	TileDebugMode debugMode = TileDebugMode::NEARBY;

	TileMap(){}
	TileMap
	(unsigned int tilePixelWidth, unsigned int tilePixelHeight,
	 float tileScaleX, float tileScaleY) :
		_tilePixelWidth(tilePixelWidth), _tilePixelHeight(tilePixelHeight),
		_tileScaleX(tileScaleX), _tileScaleY(tileScaleY)
	{

	}
	inline void SetTileScale(float tileScaleX, float tileScaleY)
	{
		_tileScaleX = tileScaleX;
		_tileScaleY = tileScaleY;
	}
	// Set the pixel width and height of the tiles in the map. 
	// They need to be exactly the amount of the ones in the texture.
	inline void SetTilePixels(unsigned int tilePixelWidth, unsigned int tilePixelHeight)
	{
		_tilePixelWidth = tilePixelWidth;
		_tilePixelHeight = tilePixelHeight;
	}
	inline bool InRange(unsigned int x, unsigned int y);

	Type GetType() override
	{
		return Type::TILEMAP;
	}

	// Loads map from a .txt file
	// Map is supposed to be made of whole numbers so that it can be mapped onto the Tileset
	bool LoadTileMap(const char* filePath);

	bool SetTileSet(SDL_Texture* texture, unsigned int tileNumber);
	// ID starts at 0 -> ... 
	bool SetTileCollidable(unsigned int ID, bool isCollidable);
	// ID starts at 0 -> ...
	bool SetTileFriction(unsigned int ID, float friction);

	std::vector<std::vector<unsigned int>>& GetTiles()
	{
		return _tiles;
	}
	bool IsTileCollidable(unsigned int x, unsigned int y); 
	float GetTileFriction(unsigned int x, unsigned int y); 
	struct TileScale
	{
		unsigned int pixelWidth = 0;
		unsigned int pixelHeight = 0;
		float tileScaleX = 0.0f;
		float tileScaleY = 0.0f;
		float combinedX = 0.0f;
		float combinedY = 0.0f;
	};
	TileScale GetTileScale();
	Components::CollisionBox GetTileCollisionBox(unsigned int x, unsigned int y);

	void PrintTileMap();
	void Render();
};

class GameObject : public Object
{
public:
	Components::Transform transform = {};
private:
	std::vector<Components::Component*> objComponents;
	inline void push_back(Components::Component* component)
	{
		component->parent = this;
		component->Init(transform.position);
		objComponents.push_back(component);
	}
	static unsigned int IDNumber;
	SDL_FRect rect;
public:
	unsigned int ID;
	void SetPosition(const Vector2& pos);
	inline SDL_FRect& GetRect()
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
			transform.position.y + GetRect().h,  // maxY
			transform.position.x,                 // minX
			transform.position.x + GetRect().w    // maxX
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
	bool RemoveComponent()
	{
		for (size_t i = 0; i < objComponents.size(); i++)
		{
			if (T* component = dynamic_cast<T*>(objComponents[i]))
			{
				delete objComponents[i];
				objComponents.erase(objComponents.begin() + i);
				return true;
			}
		}
		return false;
	}
	template <typename T>
	bool HasComponent() const
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
	Vector2 position = {};
	float FOV = 0.0f;
	void follow(GameObject& other);
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

inline std::ostream& operator<<(std::ostream& os, const Components::CollisionBox& box)
{
	os << "CollisionBox(minX: " << box.minX
		<< ", maxX: " << box.maxX
		<< ", minY: " << box.minY
		<< ", maxY: " << box.maxY << ")";
	return os;
}
#pragma once
#include <vector>
#include <memory>
#include <SDL3/SDL.h>
#include <iostream>
#include "../Vectors/Vector2D.h"
#include "Components.h"
#include "Layer.h"

enum Type
{
	OBJECT,
	GAMEOBJECT,
	TILEMAP,
	LIGHT,
	OTHER
};
class Object
{
public:
	virtual ~Object() {};
	virtual Type GetType() { return Type::OBJECT; };
};
class Rendering;
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
		float friction;
		uint32_t collisionMask;
		Layer layer;
		uint8_t ID;
		bool isSolid;
	};

	std::vector<TileProperties> _tileProperties;
	std::vector<std::vector<uint8_t>> _tileGrid;
	std::vector<float> _tileLightLevel;

	SDL_Texture* textureMap = nullptr;

	TileSet _currentTileSet = {};
	unsigned int _tilePixelWidth = 0, _tilePixelHeight = 0;
	float _tileScaleX = 0.0f, _tileScaleY = 0.0f;
	TileMap::TileProperties& _GetTileProperties(unsigned int ID);
	void _SetTileLightLevel(unsigned int x, unsigned int y, float amount);
	float* _GetTileLightLevel(unsigned int x, unsigned int y);
	friend class Rendering;
public:
	enum TileDebugMode
	{
		NONE,
		NEARBY,
		FULL
	};
	struct TileScale
	{
		float tileScaleX = 0.0f;
		float tileScaleY = 0.0f;
		float combinedX = 0.0f;
		float combinedY = 0.0f;
		unsigned int pixelWidth = 0;
		unsigned int pixelHeight = 0;
	};
	Vector2 position = { 0.0f,0.0f };
	TileDebugMode debugMode = TileDebugMode::NEARBY;
	Type GetType() override
	{
		return Type::TILEMAP;
	}
	TileMap() {}
	TileMap(unsigned int tilePixelWidth, unsigned int tilePixelHeight, float tileScaleX, float tileScaleY) :
		_tilePixelWidth(tilePixelWidth), _tilePixelHeight(tilePixelHeight),
		_tileScaleX(tileScaleX), _tileScaleY(tileScaleY)
	{
	}
	inline void SetTileScale(float tileScaleX, float tileScaleY)
	{
		_tileScaleX = tileScaleX;
		_tileScaleY = tileScaleY;
	}
	inline void SetTilePixels(unsigned int tilePixelWidth, unsigned int tilePixelHeight)
	{
		_tilePixelWidth = tilePixelWidth;
		_tilePixelHeight = tilePixelHeight;
	}  	// Set the pixel width and height of the tiles in the map. They need to be exactly the amount of the ones in the texture.
	inline bool InRange(unsigned int x, unsigned int y);
	bool SetTileSet(SDL_Texture* texture, unsigned int tileNumber);

	// ID starts at 0 -> ... 
	bool SetTileSolid(unsigned int ID, bool isSolid);
	bool SetTileFriction(unsigned int ID, float friction);

	bool SetTileLayer(unsigned int ID, Layer layer);
	void AddTileCollisionLayer(unsigned int ID, Layer layer);
	void RemoveTileCollisionLayer(unsigned int ID, Layer layer);

	bool IsTileSolid(unsigned int x, unsigned int y);
	bool CanTileCollideWith(unsigned int x, unsigned int y, Components::Collider2D* other);

	float GetTileFriction(unsigned int x, unsigned int y);
	unsigned int GetTileID(unsigned int x, unsigned int y);

	uint32_t& GetTileCollisionMask(unsigned int ID);
	int GetTileLayer(unsigned int ID);
	TileScale GetTileScale();
	Components::Collider2D::CollisionBox GetTileCollisionBox(unsigned int x, unsigned int y);
	std::vector<std::vector<uint8_t>>& GetTiles()
	{
		return _tileGrid;
	}
	std::vector<float>& GetLightLevels()
	{
		return _tileLightLevel;
	}

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
	SDL_FRect rect = {};

public:
	unsigned int ID = 0;
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
	inline Components::Collider2D::CollisionBox GetCollisionBox()
	{
		if (HasComponent<Components::Collider2D>())
		{
			Components::Collider2D* coll = GetComponent<Components::Collider2D>();
			coll->collisionBox.minY = transform.position.y;
			coll->collisionBox.maxY = transform.position.y + GetRect().h;
			coll->collisionBox.minX = transform.position.x;
			coll->collisionBox.maxX = transform.position.x + GetRect().w;
			return coll->collisionBox;
		}
		return Components::Collider2D::CollisionBox
		{
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
	GameObject(const GameObject& other)	: 
		ID(++IDNumber), 
		rect(other.rect), 
		transform(other.transform)
	{
		objComponents.reserve(other.objComponents.size());
		for (auto* x : other.objComponents)
		{
			if(x)
				objComponents.push_back(x->Clone());
		}
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
class Light2D : public Object
{
public:
	Type GetType() override{
		return Type::LIGHT;
	}
	Components::Transform transform = {};
	int range = 1;
	float degrees = 0.0f;
	SDL_FColor color;

	void CastLight();
};

extern Camera camera;
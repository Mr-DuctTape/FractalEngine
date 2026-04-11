#include "Entities.h"
#include "../Core/FractalEngineCore.h"
#include "../Rendering/RenderingSystem.h"
#include "../Physics/PhysicsFunctions.h"
#include <fstream>
#include <iostream>
using namespace Components;

namespace CollisionUtils
{
	uint32_t LayerToBit(Layer layer)
	{
		return 1 << static_cast<int>(layer);
	}
	void SetLayer(Layer& layer, Layer otherLayer)
	{
		layer = otherLayer;
	}
	void AddCollisionLayer(uint32_t& collisionMask, Layer layer)
	{
		if (collisionMask == 0xFFFFFFFFu)
		{
			collisionMask = ~collisionMask;
		}
		collisionMask = collisionMask | LayerToBit(layer);
	}
	void RemoveCollisionLayer(uint32_t& collisionMask, Layer layer)
	{
		// XOR Strategy
		//0110 // old bitmask
		//0010 // Layer to remove
		//0100 // (^) operator new bitmask
		// Just needs one operator compared too two
		collisionMask = collisionMask ^ LayerToBit(layer);
	}
}

/// --- STATIC Variables ---
unsigned int GameObject::IDNumber = 0;
unsigned int& Camera::screenWidth = FractalEngineCore::width;
unsigned int& Camera::screenHeight = FractalEngineCore::height;

// --- ANIMATOR COMPONENT ---
std::unordered_map<std::string, Components::Animator::Animation> Components::Animator::_animations;
void Animator::CreateAnimation(const std::string& name,
	unsigned int frames,
	float speed,
	SDL_Texture* spriteSheet)
{
	Init();

	Animation animation;
	animation.numberOfFrames = frames;
	animation.spriteSheet = spriteSheet;

	animation.spriteHeight = spriteSheet->h;
	animation.spriteWidth = spriteSheet->w / frames;
	animation.spriteX = 0;
	animation.spriteY = 0;

	float texW = 0.0f, texH = 0.0f;
	if (SDL_GetTextureSize(spriteSheet, &texW, &texH))
	{
		sprite->u1 = animation.spriteX / texW;
		sprite->v1 = animation.spriteY / texH;
		sprite->u2 = (animation.spriteX + animation.spriteWidth) / texW;
		sprite->v2 = (animation.spriteY + animation.spriteHeight) / texH;
	}

	animation.frameIndex = 0;
	animation.timer = 0.0f;
	animation.frameTime = speed;
	_animationSpeed = speed;
	Animator::_animations.emplace(name, animation);
}
inline void Animator::Init()
{
	if (_hasInit) return;

	if (!sprite)
		sprite = parent->GetComponent<Sprite>();

	if (!sprite)
	{
		sprite = parent->AddComponent<Sprite>();
	}
	_hasInit = true;
}
void Animator::Update(const float deltaTime)
{
	auto* animation = &_currentAnimation;

	if (!animation->spriteSheet) return;

	if (animation->spriteSheet->w == 0 || animation->spriteSheet->h == 0) return;
	animation->timer += deltaTime;

	if (animation->timer >= _animationSpeed)
	{
		animation->timer -= _animationSpeed;

		animation->frameIndex++;

		if (animation->frameIndex >= animation->numberOfFrames)
			animation->frameIndex = 0;
	}

	animation->spriteX = animation->spriteWidth * animation->frameIndex;

	sprite->u1 = animation->spriteX / static_cast<float>(animation->spriteSheet->w);
	sprite->v1 = animation->spriteY / static_cast<float>(animation->spriteSheet->h);
	sprite->u2 = (animation->spriteX + animation->spriteWidth) / static_cast<float>(animation->spriteSheet->w);
	sprite->v2 = (animation->spriteY + animation->spriteHeight) / static_cast<float>(animation->spriteSheet->h);
}
void Animator::SetAnimation(const std::string& name)
{
	auto it = Animator::_animations.find(name);
	if (it == Animator::_animations.end()) return;
	Init();

	_currentAnimation = it->second;
	_currentAnimation.savedSprite = sprite->texture;
	sprite->texture = _currentAnimation.spriteSheet;
}
void Animator::SetSpeed(float speed)
{
	this->_animationSpeed = speed;
}
void Animator::Play()
{
	isPlaying = true;
}
void Animator::Stop()
{
	sprite->texture = _currentAnimation.savedSprite;
	sprite->ResetUV();
	isPlaying = false;
}
///---------------------------

// ----- GAMEOBJECT CLASS -----
void GameObject::SetPosition(const Vector2& pos)
{
	Physics2D* physComp = GetComponent<Physics2D>();
	if (!physComp)
	{
		transform.position = pos;
		return;
	}
	physComp->position_current = pos;
	physComp->position_old = pos;
	transform.position = pos;
}

// ----- CAMERA CLASS -----
void Camera::follow(GameObject& other)
{
	SDL_FRect rect = other.GetRect();
	position.x = (rect.x + rect.w) / 1.1 - screenWidth / 2;
	position.y = (rect.y + rect.h) / 1.1 - screenHeight / 2;
}
Camera camera;
/// ------- TILEMAP CLASS --------

// ---- HELPER Functions ----
bool TileMap::InRange(unsigned int x, unsigned int y)
{
	if (y >= _tileGrid.size() || x >= _tileGrid[y].size())
		return false;
	return true;
}

// --- GET Functions ----
TileMap::TileProperties& TileMap::_GetTileProperties(unsigned int ID)
{
	if (ID >= 255 || ID >= _tileProperties.size())
	{
		throw std::out_of_range("ID TOO BIG! Could not find Tile properties ID: " + std::to_string(ID));
	}
	if (ID != _tileProperties[ID].ID)
	{
		throw std::runtime_error("Could not find Tile properties ID: " + std::to_string(ID));
	}
	return _tileProperties[ID];
}
Components::Collider2D::CollisionBox TileMap::GetTileCollisionBox(unsigned int x, unsigned int y)
{
	// Scale to get Tilemap coords -> World coords
	TileMap::TileScale scale = GetTileScale();

	float worldX = (float)(position.x + x) * scale.combinedX;
	float worldY = (float)(position.y + y) * scale.combinedY;

	Components::Collider2D::CollisionBox box{
			worldY ,
			worldY + scale.combinedY,
			worldX,
			worldX + scale.combinedX };

	return box;
}
unsigned int TileMap::GetTileID(unsigned int x, unsigned int y)
{
	if (!InRange(x, y))
		return 0;
	return _tileGrid[y][x];
}
bool TileMap::IsTileSolid(unsigned int x, unsigned int y)
{
	return _GetTileProperties(GetTileID(x, y)).isSolid;
}
float TileMap::GetTileFriction(unsigned int x, unsigned int y)
{
	return _GetTileProperties(GetTileID(x, y)).friction;
}
TileMap::TileScale TileMap::GetTileScale()
{
	TileScale scale;
	scale.pixelHeight = _tilePixelHeight;
	scale.pixelWidth = _tilePixelWidth;
	scale.tileScaleX = _tileScaleX;
	scale.tileScaleY = _tileScaleY;
	scale.combinedX = _tilePixelWidth * _tileScaleX;
	scale.combinedY = _tilePixelHeight * _tileScaleY;
	return scale;
}

uint32_t& TileMap::GetTileCollisionMask(unsigned int ID)
{
	Uint32 error = -1;
	if (ID >= 255)
		return error;
	if (ID >= _tileProperties.size())
		return error;
	return _tileProperties[ID].collisionMask;
}
Layer& TileMap::GetTileLayer(unsigned int ID)
{
	Layer layer = Layer::ERROR;
	if (ID >= 255)
		return layer;
	if (ID >= _tileProperties.size())
		return layer;
	return _tileProperties[ID].layer;
}

// ---- SET Functions -----
bool TileMap::SetTileFriction(unsigned int ID, float friction)
{
	if(ID >= 255 || ID >= _tileProperties.size())
		return false;
	if (_tileProperties[ID].ID == ID)
	{
		_tileProperties[ID].friction = friction;
		return true;
	}
	return false;
}
bool TileMap::SetTileSolid(unsigned int ID, bool isSolid)
{
	if (ID >= 255 || ID >= _tileProperties.size())
		return false;
	if (_tileProperties[ID].ID == ID)
	{
		_tileProperties[ID].isSolid = isSolid;
		return true;
	}
	return false;
}
bool TileMap::SetTileLayer(unsigned int ID, Layer layer)
{
	if (ID >= 255 || ID >= _tileProperties.size())
		return false;
	if (_tileProperties[ID].ID == ID)
	{
		_tileProperties[ID].layer = layer;
		return true;
	}
	return false;
}
void TileMap::AddTileCollisionLayer(unsigned int ID, Layer layer)
{
	TileProperties& properties = _GetTileProperties(ID);
	CollisionUtils::AddCollisionLayer(properties.collisionMask, layer);
}
bool TileMap::SetTileSet(SDL_Texture* texture, unsigned int tileNumber)
{
	if (texture == nullptr)
		return false;

	if (_tileProperties.size() != 0)
		_tileProperties.clear();

	_currentTileSet.spriteSheet = texture;
	_currentTileSet.numberOfTiles = tileNumber;

	for (size_t i = 0; i < tileNumber; i++) //Create properties for each tile ID
	{
		_tileProperties.emplace_back();
		_tileProperties[i].ID = i;
	}

	if (_currentTileSet.spriteSheet == texture && _currentTileSet.numberOfTiles == tileNumber)
		return true;
	else
		return false;
}

// --- OTHER Functions ---
bool TileMap::CanTileCollideWith(unsigned int x, unsigned int y, Collider2D* other)
{
	if (!other)
		return false;

	TileProperties& properties = _GetTileProperties(GetTileID(x, y));
	if (properties.ID == -1) // Just a check for the ID
		return false;
	if (other->CanCollide(properties.collisionMask))
		return true;
	return false;
}
void TileMap::RemoveTileCollisionLayer(unsigned int ID, Layer layer)
{
	TileProperties& properties = _GetTileProperties(ID);
	CollisionUtils::RemoveCollisionLayer(properties.collisionMask, layer);
}

// --- MAP Functions ----
void TileMap::Render()
{
	//Do simple rendering before even trying the verticies and indices
	if (!_currentTileSet.spriteSheet)
		return;

	if (_tileGrid.size() <= 0)
		return;

	TileMap::TileScale scale = GetTileScale();

	SDL_FRect srcRect = {};
	srcRect.w = scale.pixelWidth;
	srcRect.h = scale.pixelHeight;
	srcRect.y = 0;

	for (size_t y = 0; y < _tileGrid.size(); y++)
	{
		for (size_t x = 0; x < _tileGrid[y].size(); x++)
		{
			SDL_FRect dstRect{};
			dstRect.w = (int)scale.combinedX;
			dstRect.h = (int)scale.combinedY;
			dstRect.x = (int)(position.x + (x * dstRect.w)) - camera.position.x;
			dstRect.y = (int)(position.y + (y * dstRect.h)) - camera.position.y;

			unsigned int tileID = _tileGrid[y][x];
			if (tileID > _currentTileSet.numberOfTiles)
				tileID = _currentTileSet.numberOfTiles - 1;

			srcRect.x = srcRect.w * tileID;
			SDL_RenderTexture(Rendering::GetRenderer(), _currentTileSet.spriteSheet, &srcRect, &dstRect);
			if (!IsTileSolid(x, y) && debugMode == TileDebugMode::FULL)
			{
				auto box = GetTileCollisionBox(x, y);
				Rendering::Debug::DrawCollisionBox(box, { 255, 255, 255, 255 }, false);
			}
			else if (IsTileSolid(x, y) && debugMode == TileDebugMode::FULL)
			{
				auto box = GetTileCollisionBox(x, y);
				Rendering::Debug::DrawCollisionBox(box, { 255, 0, 0, 96 }, true);
			}
		}
	}
}

// --- COMPONENTS ---
uint32_t Collider2D::LayerToBit(Layer layer)
{
	return 1 << static_cast<int>(layer);
}

void Collider2D::SetLayer(Layer layer)
{
	this->layer = layer;
}

void Collider2D::AddCollisionLayer(Layer layer)
{
	if (collisionMask == 0xFFFFFFFFu)
	{
		collisionMask = ~collisionMask;
	}
	collisionMask = collisionMask | LayerToBit(layer);
}

void Collider2D::RemoveCollisionLayer(Layer layer)
{
	// XOR Strategy
	//0110 // old bitmask
	//0010 // Layer to remove
	//0100 // (^) operator new bitmask
	// Just needs one operator compared too two
	collisionMask = collisionMask ^ LayerToBit(layer);
}

bool Collider2D::CanCollide(const Collider2D& other)
{
	return (collisionMask & other.collisionMask) != 0; // Compare bits
}

bool Collider2D::CanCollide(const uint32_t& collisionMask)
{
	return (this->collisionMask & collisionMask) != 0;
}

Layer& Collider2D::GetLayer()
{
	return layer;
}
uint32_t& Collider2D::GetCollisionMask()
{
	return collisionMask;
}
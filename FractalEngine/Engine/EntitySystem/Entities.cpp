#include "Entities.h"
#include "../Core/FractalEngineCore.h"
#include "../Rendering/RenderingSystem.h"
#include "../Physics/PhysicsFunctions.h"
#include <fstream>
#include <iostream>

using namespace Components;

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
	if (y >= _tiles.size() || x >= _tiles[y].size())
		return false;
	return true;
}

// --- GET Functions ----

TileMap::TileProperties TileMap::_GetTileProperties(unsigned int ID)
{
	if (ID >= _tileProperties.size())
	{
		std::cout << "ID TOO BIG! Could not find Tile properties ID: " << ID << "\n";
		return TileMap::TileProperties{};
	}
	if (ID == _tileProperties[ID].ID)
	{
		return _tileProperties[ID];
	}

	std::cout << "Could not find Tile properties ID: " << ID << "\n";
	return TileMap::TileProperties{};
}
Components::CollisionBox TileMap::GetTileCollisionBox(unsigned int x, unsigned int y)
{
	// Scale to get Tilemap coords -> World coords
	TileMap::TileScale scale = GetTileScale();

	float worldX = (float)(position.x + x) * scale.combinedX;
	float worldY = (float)(position.y + y) * scale.combinedY;

	Components::CollisionBox box{
			worldY ,                
			worldY + scale.combinedY,  
			worldX,                
			worldX + scale.combinedX};

	return box;
}
bool TileMap::IsTileCollidable(unsigned int x, unsigned int y)
{
	if (!InRange(x, y))
		return false;

	TileProperties properties = _GetTileProperties(_tiles[y][x]);
	return properties.isCollidable;
}
float TileMap::GetTileFriction(unsigned int x, unsigned int y)
{
	if (!InRange(x, y))
		return 0.0f;

	TileProperties properties = _GetTileProperties(_tiles[y][x]);
	return properties.friction;
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

// ---- SET Functions -----
bool TileMap::SetTileFriction(unsigned int ID, float friction)
{
	if (ID >= _tileProperties.size())
		return false;
	if (_tileProperties[ID].ID == ID)
	{
		_tileProperties[ID].friction = friction;
		return true;
	}
	return false;
}
bool TileMap::SetTileCollidable(unsigned int ID, bool isCollidable)
{
	if (ID >= _tileProperties.size())
		return false;
	if (_tileProperties[ID].ID == ID)
	{
		_tileProperties[ID].isCollidable = isCollidable;
		return true;
	}
	return false;
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

// --- MAP Functions ----
void TileMap::PrintTileMap()
{
	for (size_t i = 0; i < _tiles.size(); i++)
	{
		for (size_t j = 0; j < _tiles[i].size(); j++)
		{
			if (_tiles[i][j] > _currentTileSet.numberOfTiles)
			{
				std::cout << "Tile read failure i: " << i << " j: " << j << "\n";
				return;
			}
			std::cout << _tiles[i][j];
		}
		std::cout << "\n";
	}
}
bool TileMap::LoadTileMap(const char* filePath) // TODO: Make a better system for loading tilemaps from files
{
	if (!filePath) return false;
	std::string line;
	std::ifstream readFile(filePath);
	if (!readFile) return false;

	unsigned int y = 0;
	while (std::getline(readFile, line))
	{
		if (!line.empty() && std::isdigit(line[0]))
		{
			if (_tiles.size() <= y)
			{
				_tiles.emplace_back();
			}
			for (size_t i = 0; i < line.size(); i++)
			{
				if (!std::isdigit(line[0]))
					continue;

				int number = line[i] - '0';
				_tiles[y].push_back(number);
			}
			y++;
		}
	}
	return true;
}
void TileMap::Render()
{
	//Do simple rendering before even trying the verticies and indices
	if (!_currentTileSet.spriteSheet)
		return;

	if (_tiles.size() <= 0)
		return;

	TileMap::TileScale scale = GetTileScale();

	SDL_FRect srcRect = {};
	srcRect.w = scale.pixelWidth;
	srcRect.h = scale.pixelHeight;
	srcRect.y = 0;

	for (size_t y = 0; y < _tiles.size(); y++)
	{
		for (size_t x = 0; x < _tiles[y].size(); x++)
		{
			SDL_FRect dstRect{};
			dstRect.w = (int)scale.combinedX;
			dstRect.h = (int)scale.combinedY;
			dstRect.x = (int)(position.x + (x * dstRect.w)) - camera.position.x;
			dstRect.y = (int)(position.y + (y * dstRect.h)) - camera.position.y;

			unsigned int tileID = _tiles[y][x];
			if (tileID > _currentTileSet.numberOfTiles)
				tileID = _currentTileSet.numberOfTiles - 1;

			srcRect.x = srcRect.w * tileID;
			SDL_RenderTexture(Rendering::GetRenderer(), _currentTileSet.spriteSheet, &srcRect, &dstRect);

			if (!IsTileCollidable(x, y) && debugMode == TileDebugMode::FULL)
			{
				auto box = GetTileCollisionBox(x, y);
				Rendering::Debug::DrawCollisionBox(box, { 255, 255, 255, 255 }, false);
			}
			else if (IsTileCollidable(x, y) && debugMode == TileDebugMode::FULL)
			{
				auto box = GetTileCollisionBox(x, y);
				Rendering::Debug::DrawCollisionBox(box, { 255, 0, 0, 96 }, true);
			}
		}
	}
}

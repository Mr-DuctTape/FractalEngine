#include "Entities.h"
#include "../Core/FractalEngineCore.h"
#include "../Rendering/RenderingSystem.h"
#include "../Physics/PhysicsFunctions.h"
#include "../SceneManagement/FractalScene.h"
#include "Components.h"

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
	bool foundObject = false;
	for (auto& obj : SceneManager::GetCurrentScene()->objects)
	{
		if (obj->GetType() != Type::GAMEOBJECT)
			continue;

		GameObject* ob = static_cast<GameObject*>(obj);
		if (ob == &other)
		{
			foundObject = true;
		}
	}
	if (!foundObject)
		return;

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
float* TileMap::_GetTileLightLevel(unsigned int x, unsigned int y)
{
	if (!InRange(x, y))
		return nullptr;

	unsigned int width = _tileGrid[y].size();
	return &_tileLightLevel[y * width + x];
}
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
int TileMap::GetTileLayer(unsigned int ID)
{
	if (ID >= 255)
		return -1;
	if (ID >= _tileProperties.size())
		return -1;
	return static_cast<int>(_tileProperties[ID].layer);
}

// ---- SET Functions -----
void TileMap::_SetTileLightLevel(unsigned int x, unsigned int y, float amount)
{
	if (!InRange(x, y))
		return;

	unsigned int width = _tileGrid[y].size();
	_tileLightLevel[y * width + x] = amount;
}
bool TileMap::SetTileFriction(unsigned int ID, float friction)
{
	if (ID >= 255 || ID >= _tileProperties.size())
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

struct Tile
{
	SDL_FRect rect;
	float lightLevel;
	int ID;
};

// --- MAP Functions ----
void TileMap::Render()
{
	//Do simple rendering before even trying the verticies and indices
	if (!_currentTileSet.spriteSheet)
		return;

	if (_tileGrid.size() <= 0)
		return;

	TileMap::TileScale scale = GetTileScale();

	unsigned int sizeY = _tileGrid.size();
	unsigned int sizeX = _tileGrid[0].size();

	if (!textureMap || textureMap->w != sizeX * scale.combinedX || textureMap->h != sizeY * scale.combinedY)
	{
		SDL_DestroyTexture(textureMap);
		textureMap = SDL_CreateTexture(Rendering::GetRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, sizeX * scale.combinedX, sizeY * scale.combinedY);
	}
	else
	{
		if (debugMode != TileDebugMode::FULL)
			return;
		for (size_t y = 0; y < _tileGrid.size(); y++)
			for (size_t x = 0; x < _tileGrid[y].size(); x++)
			{
				if (!IsTileSolid(x, y))
				{
					auto box = GetTileCollisionBox(x, y);
					Rendering::Debug::DrawCollisionBox(box, { 255, 255, 255, 255 }, false);
				}
				else if (IsTileSolid(x, y))
				{
					auto box = GetTileCollisionBox(x, y);
					Rendering::Debug::DrawCollisionBox(box, { 255, 0, 0, 96 }, true);
				}
			}

		return;
	}

	SDL_FRect srcRect = {};
	srcRect.w = scale.pixelWidth;
	srcRect.h = scale.pixelHeight;
	srcRect.y = 0;

	std::vector<Tile> Tiles;
	Tiles.reserve(1000);

	for (size_t y = 0; y < _tileGrid.size(); y++)
	{
		for (size_t x = 0; x < _tileGrid[y].size(); x++)
		{
			SDL_FRect dstRect{};
			dstRect.w = (int)scale.combinedX;
			dstRect.h = (int)scale.combinedY;
			dstRect.x = (int)(position.x + (x * dstRect.w));
			dstRect.y = (int)(position.y + (y * dstRect.h));

			unsigned int tileID = _tileGrid[y][x];
			if (tileID > _currentTileSet.numberOfTiles)
				tileID = _currentTileSet.numberOfTiles - 1;

			float* lightLevel = _GetTileLightLevel(x, y);

			Tile tile{ dstRect, *lightLevel, tileID };
			Tiles.push_back(tile);

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

	SDL_SetRenderTarget(Rendering::GetRenderer(), textureMap);
	SDL_RenderClear(Rendering::GetRenderer());
	for (auto& T : Tiles)
	{
		SDL_Color color{ 255,255,255,255 };
		srcRect.x = srcRect.w * T.ID;
		float light = (T.lightLevel + 1.0f) * 0.5f;
		color = {
			(Uint8)(color.r * light),
			(Uint8)(color.g * light),
			(Uint8)(color.b * light),
			color.a
		};
		SDL_SetTextureColorMod(_currentTileSet.spriteSheet, color.r, color.g, color.b);
		SDL_RenderTexture(Rendering::GetRenderer(), _currentTileSet.spriteSheet, &srcRect, &T.rect);
	}
	SDL_SetRenderTarget(Rendering::GetRenderer(), NULL);
}

void Light2D::CastLight()
{

}
#include "RenderingSystem.h"
#include "../Core/FractalEngineCore.h"
#include "../EntitySystem/Entities.h"
#include "../SceneManagement/FractalScene.h"
#include <iostream>
#include <algorithm>

float Rendering::cosTable[360] = {};
float Rendering::sinTable[360] = {};

int Rendering::Batch::batchNumber = 0;
SDL_Renderer* Rendering::m_Renderer = nullptr;
std::vector<std::unique_ptr<Rendering::Batch>> Rendering::m_Batches = {};
std::vector<Rendering::Line> Rendering::m_Lines = {};
std::vector<Rendering::Debug::Box> Rendering::Debug::collisionBoxes = {};

Rendering::RenderingLayer Rendering::m_lightLayer;
Rendering::RenderingLayer Rendering::m_spriteLayer;
Rendering::Batch Rendering::m_ShadowBatch = {};

SDL_Texture* Rendering::lightTexture = nullptr;

unsigned int Rendering::lightUpdateInterval = 2;

bool Rendering::Debug::RenderLight = false;

void Rendering::SetLightUpdateInterval(unsigned int interval)
{
	lightUpdateInterval = interval;
}

void Rendering::Debug::DrawCollisionBox(const Components::Collider2D::CollisionBox& box, const SDL_Color& color, bool solid)
{
	SDL_FRect rect;
	rect.x = box.minX - camera.position.x;
	rect.y = box.minY - camera.position.y;
	rect.w = box.maxX - box.minX;
	rect.h = box.maxY - box.minY;

	Rendering::Debug::Box a;
	a.rect = rect;
	a.color = color;
	a.solid = solid;

	collisionBoxes.push_back(a);
}

SDL_FColor toFColor(const SDL_Color& c)
{
	return SDL_FColor{
		c.r / 255.0f,
		c.g / 255.0f,
		c.b / 255.0f,
		c.a / 255.0f
	};
}

void Rendering::ClearScreen(SDL_Color color)
{
	SDL_Color preColor;
	SDL_GetRenderDrawColor(Rendering::m_Renderer, &preColor.r, &preColor.g, &preColor.b, &preColor.a);
	SDL_SetRenderDrawColor(Rendering::m_Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(Rendering::m_Renderer);
	SDL_SetRenderDrawColor(Rendering::m_Renderer, preColor.r, preColor.g, preColor.b, preColor.a);
}

void Rendering::DrawLine(float x1, float v1, float x2, float v2, SDL_Color color)
{
	for (const auto& line : m_Lines)
	{
		if (line.x1 == x1 && line.x2 == x2 && line.v1 == v1 && line.v2 == v2)
			return;
	}
	Rendering::m_Lines.emplace_back(Line{ x1, v1, x2, v2, color });
}

Rendering::Batch* Rendering::_CreateBatch(SDL_Texture* texture)
{
	auto batch = std::make_unique<Batch>();
	auto ptr = batch.get();
	ptr->scene = SceneManager::GetCurrentScene();
	ptr->texture = (texture) ? texture : nullptr;
	m_Batches.push_back(std::move(batch));
	return ptr;
}

Rendering::Batch* Rendering::_FindBatch(SDL_Texture* texture)
{
	for (size_t i = 0; i < m_Batches.size(); i++)
	{
		Rendering::Batch* obj = m_Batches[i].get();
		if (obj->texture == texture)
			return obj;
	}
	return nullptr;
}

Rendering::Batch* Rendering::_FindOrCreateBatch(Components::Sprite* sprite)
{
	Rendering::Batch* currentBatch = _FindBatch((sprite) ? sprite->texture : nullptr);

	if (!currentBatch)
		currentBatch = _CreateBatch((sprite) ? sprite->texture : nullptr);

	return currentBatch;
}

void Rendering::DrawQuad(float x, float y, float w, float h, float rotation, Components::Sprite* sprite, const SDL_Color& color)
{
	Rendering::Batch* currentBatch = Rendering::_FindOrCreateBatch(sprite);

	SDL_FColor quadColor = toFColor(color);

	const int startIndex = currentBatch->_Vertices.size();
	int indices[6] =
	{
		0 + startIndex,1 + startIndex,2 + startIndex, //First triangle
		2 + startIndex,1 + startIndex,3 + startIndex //Second triangle
	};
	for (size_t i = 0; i < 6; i++)
	{
		currentBatch->_Indices.push_back(indices[i]);
	}

	const int index = static_cast<int>(rotation) % 360;
	const float cos = cosTable[index];
	const float sin = sinTable[index];

	float posX = x - camera.position.x;
	float posY = y - camera.position.y;
	const SDL_FPoint center = { w / 2.0 + posX,  h / 2.0 + posY };
	SDL_Vertex vertices[4];

	vertices[0].position = { posX, posY };
	vertices[1].position = { posX + w, posY };
	vertices[2].position = { posX, posY + h };
	vertices[3].position = { posX + w, posY + h };

	if (sprite)
	{
		float u1 = sprite->u1; // left
		float u2 = sprite->u2; // right
		float v1 = sprite->v1; // top
		float v2 = sprite->v2; // botto

		if (sprite->flippedX)
		{
			float temp = u1;
			u1 = u2;
			u2 = temp;
		}

		vertices[0].tex_coord = { u1, v1 }; // top-left
		vertices[1].tex_coord = { u2, v1 }; // top-right
		vertices[2].tex_coord = { u1, v2 }; // bottom-left
		vertices[3].tex_coord = { u2, v2 }; // bottom-right
	}
	for (size_t i = 0; i < 4; i++)
	{
		//Rotation
		float vx = vertices[i].position.x;
		float vy = vertices[i].position.y;

		vertices[i].position.x = (vx - center.x) * cos - (vy - center.y) * sin + center.x;
		vertices[i].position.y = (vx - center.x) * sin + (vy - center.y) * cos + center.y;
		//

		vertices[i].color = quadColor;
		currentBatch->_Vertices.push_back(vertices[i]);
	}
}

SDL_Texture* CreateRadialLightTexture(SDL_Renderer* renderer, int size)
{
	SDL_Texture* texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ABGR8888,
		SDL_TEXTUREACCESS_STREAMING,
		size,
		size
	);

	void* pixels;
	int pitch;

	SDL_LockTexture(texture, NULL, &pixels, &pitch);

	Uint32* pixelData = (Uint32*)pixels;

	int radius = size / 2;

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			float dx = x - radius;
			float dy = y - radius;

			float distance = sqrtf(dx * dx + dy * dy);
			float normalized = distance / radius;

			// Clamp 0–1
			if (normalized > 1.0f) normalized = 1.0f;

			// Invert so center = 1, edge = 0
			float intensity = 1.0f - normalized;

			// Optional: smoother falloff
			intensity = intensity * intensity;

			Uint8 value = (Uint8)(intensity * 255);

			// ABGR8888
			pixelData[y * (pitch / 4) + x] =
				(255 << 24) |  // A
				(value << 16) | // B
				(value << 8) | // G
				(value);        // R
		}
	}

	SDL_UnlockTexture(texture);

	// IMPORTANT for lighting
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);

	return texture;
}

void Rendering::RenderLights(std::vector<Object*>* objects, bool& lightsInScene)
{
	if (!objects)
	{
		objects = &SceneManager::GetCurrentScene()->objects;
	}

	for (auto& obj : *objects)
	{
		if (obj->GetType() != Type::LIGHT)
			continue;
		lightsInScene = true;
		Light2D* light = static_cast<Light2D*>(obj);

		SDL_FRect lightRect;
		float scaleX = (float)m_lightLayer.texture->w / FractalEngineCore::width;
		float scaleY = (float)m_lightLayer.texture->h / FractalEngineCore::height;

		lightRect.x = (light->transform.position.x - light->range - camera.position.x) * scaleX;
		lightRect.y = (light->transform.position.y - light->range - camera.position.y) * scaleY;
		lightRect.w = (light->range * 2) * scaleX;
		lightRect.h = (light->range * 2) * scaleY;

		if (lightRect.x + lightRect.w < 0 || lightRect.x > FractalEngineCore::width ||
			lightRect.y + lightRect.h < 0 || lightRect.y > FractalEngineCore::height)
		{
			continue;
		}

		SDL_RenderTexture(Rendering::GetRenderer(), lightTexture, NULL, &lightRect);
	}
}



void Rendering::PushToScreen()
{
	if (!Rendering::m_Renderer)
		return;
	static unsigned int lightUpdateFrame = 0;
	static bool lightsInScene = false;
	lightUpdateFrame++;

	if (!lightTexture)
	{
		lightTexture = CreateRadialLightTexture(Rendering::GetRenderer(), 32);
		SDL_SetTextureScaleMode(lightTexture, SDL_SCALEMODE_LINEAR);
	}

	if (m_lightLayer.texture == nullptr || m_spriteLayer.texture == nullptr)
	{
		m_lightLayer.texture = SDL_CreateTexture(Rendering::GetRenderer(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, FractalEngineCore::width / 2, FractalEngineCore::height / 2);
		m_spriteLayer.texture = SDL_CreateTexture(Rendering::GetRenderer(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, FractalEngineCore::width, FractalEngineCore::height);
		SDL_SetTextureBlendMode(lightTexture, SDL_BLENDMODE_ADD);
	}
	if (m_lightLayer.texture->w != FractalEngineCore::width / 2 || m_lightLayer.texture->h != FractalEngineCore::height / 2 ||
		m_spriteLayer.texture->w != FractalEngineCore::width || m_spriteLayer.texture->h != FractalEngineCore::height)
	{
		SDL_DestroyTexture(m_lightLayer.texture);
		SDL_DestroyTexture(m_spriteLayer.texture);
		m_lightLayer.texture = SDL_CreateTexture(Rendering::GetRenderer(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, FractalEngineCore::width / 2, FractalEngineCore::height / 2);
		m_spriteLayer.texture = SDL_CreateTexture(Rendering::GetRenderer(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, FractalEngineCore::width, FractalEngineCore::height);
		SDL_SetTextureBlendMode(lightTexture, SDL_BLENDMODE_ADD);
	}

	SDL_Color color;
	SDL_GetRenderDrawColor(Rendering::m_Renderer, &color.r, &color.g, &color.b, &color.a);
	SDL_SetRenderTarget(Rendering::GetRenderer(), m_spriteLayer.texture);
	SDL_RenderClear(Rendering::GetRenderer());

	// World drawing and sprites
	auto& objects = SceneManager::GetCurrentScene()->objects;
	for (auto& obj : objects)
	{
		if (obj->GetType() != TILEMAP)
			continue;
		TileMap* tileMap = static_cast<TileMap*>(obj);
		tileMap->Render();
		SDL_FRect dstRect;
		dstRect.x = tileMap->position.x - camera.position.x;
		dstRect.y = tileMap->position.y - camera.position.y;
		dstRect.w = tileMap->textureMap->w;
		dstRect.h = tileMap->textureMap->h;
		SDL_SetRenderTarget(Rendering::GetRenderer(), m_spriteLayer.texture);
		SDL_RenderTexture(Rendering::GetRenderer(), tileMap->textureMap, NULL, &dstRect);
	}
	for (auto& b : m_Batches)
	{
		auto currentBatch = b.get();
		if (!currentBatch)
			continue;
		if (currentBatch->texture == nullptr)
		{
			SDL_RenderGeometry(
				Rendering::GetRenderer(),
				NULL, //Texture to use
				currentBatch->_Vertices.data(),
				currentBatch->_Vertices.size(),
				currentBatch->_Indices.data(),
				currentBatch->_Indices.size());
		}
		else if (currentBatch->texture)
		{
			SDL_RenderGeometry(
				Rendering::GetRenderer(),
				currentBatch->texture, //Texture to use
				currentBatch->_Vertices.data(),
				currentBatch->_Vertices.size(),
				currentBatch->_Indices.data(),
				currentBatch->_Indices.size());
		}
		currentBatch->_Vertices.clear();
		currentBatch->_Indices.clear();
	}
	if (!m_Lines.empty())
		for (size_t i = 0; i < m_Lines.size(); i++)
		{
			SDL_SetRenderDrawColor(Rendering::m_Renderer, m_Lines[i].color.r, m_Lines[i].color.g, m_Lines[i].color.b, m_Lines[i].color.a);
			SDL_RenderLine(Rendering::m_Renderer, m_Lines[i].x1, m_Lines[i].v1, m_Lines[i].x2, m_Lines[i].v2);
		}
	m_Lines.clear();

	// Scene lighting, update every interval
	if (lightUpdateFrame >= 0)
	{
		SDL_SetRenderTarget(Rendering::GetRenderer(), m_lightLayer.texture); 
		if (lightsInScene)
		{
			SDL_SetRenderDrawColor(Rendering::GetRenderer(), 65, 65, 65, 255);
		}
		else
		{
			SDL_SetRenderDrawColor(Rendering::GetRenderer(), 255, 255, 255, 255);
		}
		SDL_RenderClear(Rendering::GetRenderer());
		SDL_SetRenderDrawColor(Rendering::GetRenderer(), 255, 255, 255, 255); // full light
		Rendering::RenderLights(nullptr, lightsInScene);
		lightUpdateFrame = 0;
	}

	//TileLightCalculations(); ??? Keep or delete, tilebased lighting

	SDL_FRect screenRect;
	screenRect.x = 0;
	screenRect.y = 0;
	screenRect.w = FractalEngineCore::width;
	screenRect.h = FractalEngineCore::height;

	// Add layers together
	SDL_SetRenderTarget(Rendering::GetRenderer(), NULL);
	SDL_RenderTexture(Rendering::GetRenderer(), m_spriteLayer.texture, NULL, &screenRect);
	SDL_SetTextureBlendMode(m_lightLayer.texture, SDL_BLENDMODE_MOD);
	SDL_RenderTexture(Rendering::GetRenderer(), m_lightLayer.texture, NULL, &screenRect);

	//Debug drawing
	for (auto& box : Debug::GetCollisionBoxes())
	{
		SDL_SetRenderDrawColor(Rendering::GetRenderer(), box.color.r, box.color.g, box.color.b, box.color.a);
		if (box.solid)
			SDL_RenderFillRect(Rendering::GetRenderer(), &box.rect);
		else
			SDL_RenderRect(Rendering::GetRenderer(), &box.rect);
	}

	Debug::GetCollisionBoxes().clear();
	SDL_SetRenderDrawColor(Rendering::m_Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderPresent(Rendering::m_Renderer);
}
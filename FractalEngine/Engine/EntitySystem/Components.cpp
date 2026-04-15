#include "Components.h"
#include "../Rendering/RenderingSystem.h"
#include "../EntitySystem/Entities.h"

using namespace Components;


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
#include "Entities.h"
#include "../Core/FractalEngineCore.h"
#include "../Rendering/RenderingSystem.h"

using namespace Components;

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
	std::cout << _animationSpeed << "\n";
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

unsigned int GameObject::IDNumber = 0;

Camera camera;
unsigned int& Camera::screenWidth = FractalEngineCore::width;
unsigned int& Camera::screenHeight = FractalEngineCore::height;
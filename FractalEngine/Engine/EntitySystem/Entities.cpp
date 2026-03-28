#include "Entities.h"
#include "../Core/FractalEngineCore.h"
#include "../Rendering/RenderingSystem.h"

using namespace Components;

std::unordered_map < std::string, Components::Animator::Animation> Components::Animator::animations;

void Animator::createAnimation(const std::string& name,
	unsigned int frames,
	float speed,
	SDL_Texture* spriteSheet)
{
	Animation animation;
	animation.numberOfFrames = frames;
	animation.spriteSheet = spriteSheet;

	animation.frame.x = 0;
	animation.frame.y = 0;
	animation.frame.h = spriteSheet->h;
	animation.frame.w = spriteSheet->w / frames;

	animation.frameIndex = 0;
	animation.timer = 0.0f;
	animation.frameTime = speed;

	animation.renderTarget = &parent->getRenderTarget();
	Animator::animations.emplace(name, animation);
}

void Animator::Update(const float deltaTime)
{
	if (!currentAnimation) return;
	
	auto* anim = currentAnimation;
	anim->timer += deltaTime;

	if(anim->timer >= anim->frameTime)
	{
		anim->timer -= anim->frameTime;

		anim->frameIndex++;

		if (anim->frameIndex >= anim->numberOfFrames)
			anim->frameIndex = 0;
	}
}

void Animator::setAnimation(const std::string& name)
{
	auto it = Animator::animations.find(name);
	if (it == Animator::animations.end()) return;

	currentAnimation = &it->second;
}

void Animator::setSpeed(const float& speed)
{
	animationSpeed = speed;
}

void Animator::Play()
{
	isPlaying = true;
}

void Animator::Stop()
{
	isPlaying = false;
}

unsigned int GameObject::IDNumber = 0;

Camera camera;
unsigned int& Camera::screenWidth = FractalEngineCore::width;
unsigned int& Camera::screenHeight = FractalEngineCore::height;
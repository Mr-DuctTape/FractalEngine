#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <unordered_map>
#include <string>
#include "../Vectors/Vector2D.h"
#include "Layer.h"

class Rendering;
class GameObject;

namespace Components
{
	struct Component
	{
		GameObject* parent = nullptr;
		virtual ~Component() {};
		virtual void Init(const Vector2 pos) {};
		virtual Component* Clone() = 0;
	};
	struct Transform : public Component
	{
		Vector2 position;
		float rotation = 0.0;
		Transform* Clone() override
		{
			return new Transform(*this);
		}
	};
	struct Sprite : public Component
	{
		SDL_Texture* texture = nullptr;

		float u1 = 0.0f;
		float v1 = 0.0f;
		float u2 = 1.0f;
		float v2 = 1.0f;

		SDL_Color color = { 255, 255, 255, 255 };
		bool flippedX = false;
		bool flippedY = false;

		void ResetUV()
		{
			u1 = 0.0f;
			v1 = 0.0f;
			u2 = 1.0f;
			v2 = 1.0f;
		}

		Sprite* Clone() override
		{
			return new Sprite(*this);
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
		bool isGrounded = false;

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

		Physics2D* Clone() override
		{
			return new Physics2D(*this);
		}
	};
	class Animator : public Component
	{
	public:
		struct Animation
		{
			SDL_Texture* spriteSheet = nullptr;
			SDL_Texture* savedSprite = nullptr;

			float timer = 0.0;
			float frameTime = 0.0;

			unsigned int numberOfFrames = 0;
			unsigned int frameIndex = 0;

			unsigned int spriteY = 0;
			unsigned int spriteX = 0;

			unsigned int spriteWidth = 0;
			unsigned int spriteHeight = 0;
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
		void CreateAnimation(const std::string& name,unsigned int frames,float speed,SDL_Texture* spriteSheet);
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

		Animator* Clone() override
		{
			return new Animator(*this);
		}
	};
	class Collider2D : public Component
	{
	public:
		Collider2D() {}
		Collider2D(float mnY, float mxY, float mnX, float mxX)
		{
			collisionBox.minX = mnX;
			collisionBox.minY = mnY;
			collisionBox.maxX = mxX;
			collisionBox.maxY = mxY;
		}
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
		CollisionBox collisionBox = {};
	private:
		Layer layer = Layer::DEFAULT;
		uint32_t collisionMask = 0xFFFFFFFFu; // Set all bits 
		inline uint32_t LayerToBit(Layer layer);
	public:
		void SetLayer(Layer _layer);
		void AddCollisionLayer(Layer layer);
		void RemoveCollisionLayer(Layer layer);
		bool CanCollide(const Collider2D& other);
		bool CanCollide(const uint32_t& collisionMask);
		Layer& GetLayer();
		uint32_t& GetCollisionMask();
		Collider2D* Clone()
		{
			return new Collider2D(*this);
		}
	};
}

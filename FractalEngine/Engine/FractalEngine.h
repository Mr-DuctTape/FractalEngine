#pragma once

#include "Vectors/Vector2D.h"
#include "EntitySystem/Entities.h"
#include "Input/InputSystem.h"
#include "Rendering/RenderingSystem.h"
#include "Textures/TextureHandling.h"
#include "Physics/PhysicsFunctions.h"
#include "Core/FractalEngineCore.h"
#include "SceneManagement/FractalScene.h"
#include <thread>

class FractalEngine
{
private:
	static inline void Physics()
	{
		SceneManager::GetCurrentScene()->Update();
	}
	static inline void Rendering()
	{
		SceneManager::GetCurrentScene()->Render();
	}
public:
	static bool running;
	static inline void Run()
	{
		FractalEngineCore::DeltaTime();
		Input::Process();
		FractalEngine::Physics();
		FractalEngine::Rendering();
	}
	//Creates a Window and scene if not already created one, Scene name "Default", Window name "Window"
	static inline void Initialize(const char* title = "Window", const int width = 1920, const int height = 1080, const SDL_WindowFlags theme = SDL_WINDOW_RESIZABLE)
	{
		running = true;
		FractalEngineCore::CreateWindow(title, width, height, theme);
	}
	static inline void Stop()
	{
		running = false;
		AssetManager::Clear();
		SDL_Renderer* m_Renderer = Rendering::GetRenderer();
		if (m_Renderer)
			SDL_DestroyRenderer(m_Renderer);
		if (FractalEngineCore::window)
			SDL_DestroyWindow(FractalEngineCore::window);
	}
};
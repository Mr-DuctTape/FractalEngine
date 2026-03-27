#pragma once

#include "Vectors/Vector2D.h"
#include "EntitySystem/Entities.h"
#include "Input/InputSystem.h"
#include "Rendering/RenderingSystem.h"
#include "Textures/TextureHandling.h"
#include "Physics/PhysicsFunctions.h"
#include "Core/FractalEngineCore.h"
#include <thread>

class FractalEngine
{
private:
	static void Physics()
	{
		SceneManager::getCurrentScene()->Update();
	}
	static void Rendering()
	{
		SceneManager::getCurrentScene()->Render();
	}
public:
	static bool running;
	static void start()
	{
		FractalEngineCore::DeltaTime();
		Input::Process();
		FractalEngine::Physics();
		FractalEngine::Rendering();
	}
	//Creates a Window and scene if not already created one, Scene name "Default", Window name "Window"
	static void Initialize(const char* title = "Window", const int width = 1280, const int height = 720, const SDL_WindowFlags theme = SDL_WINDOW_RESIZABLE)
	{
		FractalEngineCore::CreateWindow(title, width, height, theme);
	}
	static void stop()
	{
		running = false;
		AssetManager::Clear();
		SDL_Renderer* renderer = Rendering::getRenderer();
		if (renderer)
			SDL_DestroyRenderer(renderer);
		if (FractalEngineCore::window)
			SDL_DestroyWindow(FractalEngineCore::window);
	}
};
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
		FractalEngineCore::Run();
		Input::Process();
		std::thread PhysicsThread(Physics);
		std::thread RenderThread(Rendering);

		PhysicsThread.join();
		RenderThread.join();
	}
	//Creates a scene if not already done named "Default"
	static void Initialize(const char* title = "Default", const int width = 1280, const int height = 720, const SDL_WindowFlags theme = SDL_WINDOW_BORDERLESS)
	{
		FractalEngineCore::CreateWindow(title, width, height, theme);
	}
	static void stop()
	{
		running = false;
	}
};